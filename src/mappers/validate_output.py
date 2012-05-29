import optparse
import sys

bingo = 0
total = 0
#quality = # ovo ce biti neka naprednija mjera, jednom

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
        #if (abs(mapper_from-wgsim_pos1) < 10) or (abs(mapper_from-(wgsim_pos2-len(read)+1)) < 10):
            bingo += 1
        else:
            print read
        total += 1

        print str(total) + ": " + str(mapper_from) + " " + str(wgsim_pos1) + " " + str(wgsim_pos2)

print bingo, total, 1.*bingo/total
        
