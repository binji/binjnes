#!/usr/bin/python3

import argparse
import collections
import sys
import xml.etree.ElementTree as ET

SIZES = {
  '2k': 2 * 1024,
  '4k': 4 * 1024,
  '8k': 8 * 1024,
  '16k': 16 * 1024,
  '32k': 32 * 1024,
  '64k': 64 * 1024,
  '128k': 128 * 1024,
  '256k': 256 * 1024,
  '512k': 512 * 1024,
}

PADS = {
  ('1', '0'): 'MIRROR_VERTICAL',
  ('0', '1'): 'MIRROR_HORIZONTAL',
}

BOARDS = {
  'BNROM':    'BOARD_BNROM',
  'NINA-01':  'BOARD_NINA001',
  'HKROM':    'BOARD_HKROM',
  'SNROM':    'BOARD_SNROM',
  'SOROM':    'BOARD_SOROM',
  'SUROM':    'BOARD_SUROM',
  'SXROM':    'BOARD_SXROM',
  'TBROM':    'BOARD_TXROM',
  'TEROM':    'BOARD_TXROM',
  'TFROM':    'BOARD_TXROM',
  'TGROM':    'BOARD_TXROM',
  'TKEPROM':  'BOARD_TXROM',
  'TKROM':    'BOARD_TXROM',
  'TKSROM':   'BOARD_TXROM',
  'TL1ROM':   'BOARD_TXROM',
  'TL2ROM':   'BOARD_TXROM',
  'TLROM':    'BOARD_TXROM',
  'TLSROM':   'BOARD_TXROM',
  'TNROM':    'BOARD_TXROM',
  'TQROM':    'BOARD_TXROM',
  'TR1ROM':   'BOARD_TXROM',
  'TSROM':    'BOARD_TXROM',
  'TVROM':    'BOARD_TXROM',
}

BOARDS_TXROM = {
  'MMC3A': 'BOARD_TXROM_MMC3A',
  'MMC3B': 'BOARD_TXROM_MMC3B',
  'MMC3C': 'BOARD_TXROM_MMC3C',
}

BOARDS_VRC2 = {
  ('PRG A1', 'PRG A0', 'CHR A15'): 'BOARD_VRC2A',
  ('PRG A0', 'PRG A1', 'CHR A16'): 'BOARD_VRC2B',
  ('PRG A1', 'PRG A0', 'CHR A16'): 'BOARD_VRC2C',
}

BOARDS_VRC4 = {
  ('PRG A1', 'PRG A2'): 'BOARD_VRC4A',
  ('PRG A1', 'PRG A0'): 'BOARD_VRC4B',
  ('PRG A6', 'PRG A7'): 'BOARD_VRC4C',
  ('PRG A3', 'PRG A2'): 'BOARD_VRC4D',
  ('PRG A2', 'PRG A3'): 'BOARD_VRC4E',
  ('PRG A0', 'PRG A1'): 'BOARD_VRC4F',
}

SUPPORTED_GENERIC_MAPPERS = {
  0, 1, 2, 3, 7, 9, 10, 11, 24, 26, 66, 69, 87, 206,
}

KEYS = ('name', 'mapper', 'board', 'prg', 'chip', 'chr', 'wram', 'battery',
       'vram', 'pad')

def size(els):
  return sum(SIZES[el.attrib['size']] for el in els)

def battery(els):
  return sum(SIZES[el.attrib['size']] for el in els
             if el.attrib.get('battery') == '1')

def chip_battery(chips):
  if chips:
    return max(8192 if chip.attrib.get('battery') == '1' else 0 for chip in chips)
  return 0

def get_pin(chips, chip_name, number):
  for chip in chips:
    if chip_name in chip.attrib['type']:
      pins = chip.findall('./pin')
      for pin in pins:
        if pin.attrib['number'] == number:
          return pin.attrib['function']
  return None


def get_board(name, chips):
  parts = name.split('-')
  if len(parts) >= 3:
    parts12 = f'{parts[1]}-{parts[2]}'
    if parts12 == 'VRC-2':
      pin3 = get_pin(chips, 'VRC2', '3')
      pin4 = get_pin(chips, 'VRC2', '4')
      pin22 = get_pin(chips, 'VRC2', '22')
      return BOARDS_VRC2.get((pin4, pin3, pin22))
    elif parts12 == 'VRC-4':
      pin3 = get_pin(chips, 'VRC4', '3')
      pin4 = get_pin(chips, 'VRC4', '4')
      return BOARDS_VRC4.get((pin4, pin3))
    elif parts12 in BOARDS.keys():
      return BOARDS[parts12]

  if len(parts) >= 2:
    if parts[1] in BOARDS.keys():
      if BOARDS[parts[1]] == 'BOARD_TXROM':
        for chip in chips:
          chip_type = chip.attrib['type']
          if chip_type.startswith('MMC3'):
            return BOARDS_TXROM[chip_type]
      else:
        return BOARDS[parts[1]]
  return None

def is_equal(a, b):
  for key in KEYS:
    if a.get(key) != b.get(key):
      return False
  return True

def main(args):
  parser = argparse.ArgumentParser()
  parser.add_argument('file', help='XML file name')
  options = parser.parse_args(args)

  f = open(options.file, 'rt', encoding='utf-16')
  tree = ET.fromstring(f.read())

  crcs = collections.defaultdict(list)

  for game in tree.findall('./game'):
    for cart in game.findall('./cartridge'):
      crc = cart.attrib['crc']

      for board in cart.findall('./board'):
        prgs = board.findall('./prg')
        chrs = board.findall('./chr')
        wrams = board.findall('./wram')
        vrams = board.findall('./vram')
        chips = board.findall('./chip')
        pad = board.find('./pad')

        chip_types = [chip.attrib['type'] for chip in chips]

        attr = {}
        attr['name'] = game.attrib['name']
        attr['board'] = get_board(board.attrib['type'], chips)
        attr['system'] = cart.attrib['system']
        attr['region'] = game.attrib['region']
        attr['mapper'] = int(board.attrib['mapper'])
        attr['prg'] = size(prgs)
        attr['chip'] = ', '.join(chip_types)
        attr['chr'] = size(chrs)
        attr['wram'] = size(wrams)
        attr['battery'] = max(battery(wrams), chip_battery(chips))
        attr['vram'] = size(vrams)

        if pad is not None:
          attr['pad'] = PADS[pad.attrib.get('h'), pad.attrib.get('v')]
        else:
          attr['pad'] = 'MIRROR_NONE'


        unique = True
        for other in crcs[crc]:
          diffs = set()
          if is_equal(other, attr):
            unique = False
          else:
            msg = f'>>> DIFFERENT {attr["name"]}'
            for key in KEYS:
              o = other.get(key)
              a = attr.get(key)
              if o != a:
                diffs.add(key)
                msg += f'\n\t{key}: {o} != {a}'

            if len(diffs.difference(set(['chip', 'name']))) == 0:
              unique = False

            # print(msg)

        if unique:
          crcs[crc].append(attr)

  mappers = collections.defaultdict(int)

  for crc in sorted(crcs.keys()):
    attr = crcs[crc][0]
    name = attr['name']
    board = attr['board']
    mapper = attr['mapper']
    prg = attr['prg'] >> 14      # 16KiB banks
    chr_ = attr['chr'] >> 12     # 4KiB banks
    wram = attr['wram'] >> 13    # 8KiB banks
    vram = attr['vram'] >> 12    # 4KiB banks
    bat = attr['battery'] >> 13  # 8KiB banks
    pad = attr['pad']

    if board is None:
      if mapper in SUPPORTED_GENERIC_MAPPERS:
        board = f'BOARD_MAPPER_{mapper}'
      else:
        board = 'BOARD_UNSUPPORTED'

    mappers[mapper] += 1

    print(f'  /* {attr["name"]} */')
    print(f'  {{.crc=0x{crc}, .board={board}, .mapper={mapper}, .prg={prg}, .chr={chr_}, .vram={vram}, .wram={wram}, .battery={bat}, .mirror={pad}}},')

#   for mapper, count in sorted(mappers.items(), key=lambda x: (x[1], x[0]), reverse=True):
#     print(mapper, count)

if __name__ == '__main__':
  main(sys.argv[1:])
