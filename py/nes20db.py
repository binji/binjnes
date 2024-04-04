#!/usr/bin/python3

import argparse
import collections
import sys
import xml.etree.ElementTree as ET

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

  for game in tree.findall('game'):
    name = ""
    for el in game.iter():
      if el.tag == ET.Comment:
        name = el.text
        break
    rom = game.find('rom')
    crc = rom.attrib['crc32']
    pcb = game.find('pcb')
    mapper = pcb.attrib['mapper']
    submapper = pcb.attrib['submapper']
    mirror = pcb.attrib['mirroring']
    battery = pcb.attrib['battery']
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

    assert crc not in crcs
    crcs[crc] = {
      'name': name,
      'mapper': mapper,
      'submapper': submapper,
      'mirror': mirror,
      'battery': battery,
      'prgrom': prgrom_size,
      'chrrom': chrrom_size,
      'prgram': prgram_size,
      'chrram': chrram_size,
      'prgnvram': prgnvram_size,
      'chrnvram': chrnvram_size,
    }

  for crc in sorted(crcs.keys()):
    game = crcs[crc]
    name = game['name']
    mapper = game['mapper']
    submapper = game['submapper']
    mirror = {
      'H': 'MIRROR_HORIZONTAL',
      'V': 'MIRROR_VERTICAL',
      '0': 'MIRROR_SINGLE_0',
      '1': 'MIRROR_SINGLE_1',
      '4': 'MIRROR_FOUR_SCREEN',
    }[game['mirror']]
    battery = game['battery']
    prgrom = game['prgrom']
    prgram = game['prgram']
    prgnvram = game['prgnvram']
    chrrom = game['chrrom']
    chrram = game['chrram']
    chrnvram = game['chrnvram']
    print(f'  /* {name} */')
    print(f'  {{.crc=0x{crc}, .mapper={mapper}, .submapper={submapper}, .mirror={mirror}, .battery={battery}, .prgrom={prgrom}, .prgram={prgram}, .prgnvram={prgnvram}, .chrrom={chrrom}, .chrram={chrram}, .chrnvram={chrnvram}}},')

if __name__ == '__main__':
  main(sys.argv[1:])
