#!/usr/bin/python3

import argparse
import collections
import math
import sys
import xml.etree.ElementTree as ET

MAPPERS = set([
    0, 1, 2, 3, 4, 5, 7, 9, 10, 11, 16, 18, 19, 21, 23, 24, 25, 26, 28, 30, 34,
    66, 69, 71, 78, 79, 85, 87, 118, 146, 163, 206, 210, 232,
])

class CommentedTreeBuilder(ET.TreeBuilder):
  def comment(self, data):
    self.start(ET.Comment, {})
    self.data(data)
    self.end(ET.Comment)

def main(args):
  parser = argparse.ArgumentParser()
  parser.add_argument('file', help='XML file name')
  options = parser.parse_args(args)

  with open(options.file, 'rt') as f:
     tree = ET.parse(f, parser=ET.XMLParser(target=CommentedTreeBuilder()))

  crcs = collections.defaultdict(dict)
  carts = set()
  prgrom_s = set()
  prgram_s = set()
  prgnvram_s = set()
  chrrom_s = set()
  chrram_s = set()
  chrnvram_s = set()

  cart_crcs = collections.defaultdict(list)

  for game in tree.findall('game'):
    name = ""
    for el in game.iter():
      if el.tag == ET.Comment:
        name = el.text
        break
    rom = game.find('rom')
    crc = rom.attrib['crc32']
    pcb = game.find('pcb')
    mapper = int(pcb.attrib['mapper'])

    # Don't bother including database entries for mappers we don't support
    if int(mapper) not in MAPPERS:
      continue

    submapper = int(pcb.attrib['submapper'])
    mirror = pcb.attrib['mirroring']
    battery = int(pcb.attrib['battery'])
    prgrom = game.find('prgrom')
    prgrom_size = int(prgrom.attrib['size']) if prgrom is not None else 0
    chrrom = game.find('chrrom')
    chrrom_size = int(chrrom.attrib['size']) if chrrom is not None else 0
    prgram = game.find('prgram')
    prgram_size = int(prgram.attrib['size']) if prgram is not None else 0
    chrram = game.find('chrram')
    chrram_size = int(chrram.attrib['size']) if chrram is not None else 0
    prgnvram = game.find('prgnvram')
    prgnvram_size = int(prgnvram.attrib['size']) if prgnvram is not None else 0
    chrnvram = game.find('chrnvram')
    chrnvram_size = int(chrnvram.attrib['size']) if chrnvram is not None else 0
    console = game.find('console')
    region = int(console.attrib['region']) if console is not None else 0

    prgrom_s.add(prgrom_size)
    prgram_s.add(prgram_size)
    prgnvram_s.add(prgnvram_size)
    chrrom_s.add(chrrom_size)
    chrram_s.add(chrram_size)
    chrnvram_s.add(chrnvram_size)

    assert crc not in crcs
    cart = (
      mapper,submapper,mirror,battery,
      prgrom_size,chrrom_size,prgram_size,
      chrram_size,prgnvram_size,chrnvram_size,
      region
    )
    carts.add(cart)
    cart_crcs[cart].append((name, crc))
    # crcs[crc] = { 'name': name, 'cart': cart }

  def readable_size(size):
    if size % 1024 == 0:
      return f'{size//1024}K'
    else:
      return f'{size}B'

  def size_name(size):
    return f'SIZE_{readable_size(size)}'

  sizes = sorted(prgrom_s | chrrom_s | prgram_s | chrram_s | prgnvram_s | chrnvram_s)
  print('typedef enum {')
  for size in sizes:
    print(f'  {size_name(size)},')
  print('} Size;\n')

  print('static const u32 s_sizes[] = {')
  for size in sizes:
    print(f'  {size},')
  print('};\n')

  print('''typedef struct {
  u16 mapper;
  u8 submapper;
  Size prgrom, prgram, prgnvram;
  Size chrrom, chrram, chrnvram;
  System system;
  Mirror mirror;
  bool battery;
} Cart;\n''')

  system_enum = {
    0: 'SYSTEM_NTSC',
    1: 'SYSTEM_PAL',
    2: 'SYSTEM_NTSC',  # ???
    3: 'SYSTEM_NTSC',  # ???
  }

  mirror_enum = {
    'H': 'MIRROR_HORIZONTAL',
    'V': 'MIRROR_VERTICAL',
    '0': 'MIRROR_SINGLE_0',
    '1': 'MIRROR_SINGLE_1',
    '4': 'MIRROR_FOUR_SCREEN',
  }

  carts = sorted(carts)

  print('static const Cart s_carts[] = {')
  for i, cart in enumerate(carts):
    mapper, submapper, mirror, battery, prgrom_size, chrrom_size, prgram_size, chrram_size, prgnvram_size, chrnvram_size, system = cart
    print(f'  /* {i} */ {{', end='')
    print(f'.mapper={mapper},', end='')
    if submapper:
      print(f'.submapper={submapper},', end='')
    print(f'.prgrom={size_name(prgrom_size)},', end='')
    if prgram_size:
      print(f'.prgram={size_name(prgram_size)},', end='')
    if prgnvram_size:
      print(f'.prgnvram={size_name(prgnvram_size)},', end='')
    if chrrom_size:
      print(f'.chrrom={size_name(chrrom_size)},', end='')
    if chrram_size:
      print(f'.chrram={size_name(chrram_size)},', end='')
    if chrnvram_size:
      print(f'.chrnvram={size_name(chrnvram_size)},', end='')
    print(f'.mirror={mirror_enum[mirror]},', end='')
    if battery:
      print(f'.battery={battery},', end='')
    if system:
      print(f'.system={system_enum[system]},', end='')
    print('},\n', end='')
  print('};\n')

  cart_crc_keys = sorted(cart_crcs.keys())
  print('static const u32 s_crcs[] = {')
  for i, cart in enumerate(cart_crc_keys):
    crcs = cart_crcs[cart]
    mapper, submapper, mirror, battery, prgrom_size, chrrom_size, prgram_size, chrram_size, prgnvram_size, chrnvram_size, system = cart
    print(f'/**** {i} ', end='')
    print(f'mapper={mapper} ', end='')
    if submapper:
      print(f'submapper={submapper} ', end='')
    print(f'prgrom={readable_size(prgrom_size)} ', end='')
    if prgram_size:
      print(f'prgram={readable_size(prgram_size)} ', end='')
    if prgnvram_size:
      print(f'prgnvram={readable_size(prgnvram_size)} ', end='')
    if chrrom_size:
      print(f'chrrom={readable_size(chrrom_size)} ', end='')
    if chrram_size:
      print(f'chrram={readable_size(chrram_size)} ', end='')
    if chrnvram_size:
      print(f'chrnvram={readable_size(chrnvram_size)} ', end='')
    print(f'mirror={mirror} ', end='')
    if battery:
      print(f'battery={battery} ', end='')
    if system:
      print(f'system={system_enum[system]} ', end='')
    print('*/')

    for j, (name, crc) in enumerate(sorted(crcs)):
      print(f'    0x{crc}, // {name}')
  print('};\n')

  counts = [str(len(cart_crcs[key])) for key in cart_crc_keys]
  max_count_len = max(len(x) for x in counts)
  max_offset_len = len(str(len(counts)))
  cols = math.ceil((80 - max_offset_len - 2 - 6) / (max_count_len + 2))
  print('static const u32 s_cart_crc_count[] = {')
  offset = 0
  while offset < len(counts):
    print(f'  /* {str(offset).rjust(max_offset_len)} */' +
          ', '.join(x.rjust(max_count_len) for x in counts[offset:offset+cols]) +
          ', ')
    offset += cols
  print('};\n')


if __name__ == '__main__':
  main(sys.argv[1:])
