# Copyright (C) 2012 Filip Pavetic
#
# Ovo je skripta koja sluzi za ocjenu kvalitete nasih mappera. Ispisuje broj readova koji su bili smjesteni na
# ispravnu poziciju u dna.
#
# Primjer uporabe:
# ./moj_mapper >mapper_output
# python validate_output.py --wgsim_output_file=test/dna1000_reads_wgsim_default_verbose --mapper_output_file=mapper_output
#
# --wgsim_output_file je file koji se dobio kao izlaz wgsim-a (to je alat za simuliranje ocitavanja dna)
# --mapper_output_file je izlaz mappera koji moze sadrzavati bilo sto, ali ova skripta ce uzeti u obzir samo linije oblika
#          'from <from> to <to> score <score>'
#          -podrazumijeva se da je redom za svaki read mapper ispisao jednu takvu liniju (cak ako nije uspio naci nikakvo mjesto
#               gdje da smjesti read)
#

import optparse
import sys

bingo = 0
total = 0

if __name__ == '__main__':
    parser = optparse.OptionParser()
    parser.add_option("--wgsim_output_file", action="store", type="string", dest="wgsim_output_file")
    parser.add_option("--mapper_output_file", action="store", type="string", dest="mapper_output_file")
    (options, args) = parser.parse_args(sys.argv[1:])

    print options.wgsim_output_file
    print options.mapper_output_file

    wgsim_o = open(options.wgsim_output_file, "r")
    mapper_o = open(options.mapper_output_file, "r")
    total = 0

    while True:
        wgsim_line = wgsim_o.readline();
        if wgsim_line =="":
            break

        read = wgsim_o.readline();
        read = read.strip()
        wgsim_o.readline();
        wgsim_o.readline();

        while True:
            mapper_line = mapper_o.readline();
            if mapper_line == "" or mapper_line.find("from") != -1:
                break
            
        if mapper_line == "":
            break

        wgsim_split = wgsim_line.split("_")
        wgsim_pos1 = int(wgsim_split[1])-1
        wgsim_pos2 = int(wgsim_split[2])-1

        mapper_split = mapper_line.split(" ")
        mapper_from = int(mapper_split[1])
        mapper_to = int(mapper_split[3])

        if mapper_from == wgsim_pos1 or mapper_from == wgsim_pos2-len(read)+1:
            bingo += 1
        else:
            print 'Nisam uspjesno smjestio ' + read
        total += 1

        print str(total) + ": " + str(mapper_from) + " " + str(wgsim_pos1) + " " + str(wgsim_pos2)

print 'Na dobru poziciju postavljeno je ' + str(bingo) + ' od ukupno ' + str(total) + ' readova. (' + str(100.*bingo/total) + '%)'
        
