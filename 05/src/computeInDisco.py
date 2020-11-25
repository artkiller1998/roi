import sys
import time
from collections import defaultdict
import ntpath
from disco.core import Job, result_iterator
import disco.ddfs
import re

def build_symbol_index(fname_path,params):
    import ntpath
    fname_path = str(ntpath.normpath(fname_path.decode().rstrip()))
    fname = ntpath.basename(fname_path)
    for  symbol in fname:
        yield str(symbol), str(fname_path)

def print_index(symbol_index,fout):
    file = open(fout,"w")
    file.write("-- output_index --\n")
    for it1,it2 in sorted(result_iterator(symbol_index), key=lambda tup: tup[0]):
        file.write("(" + it1 + ":" + it2 + ")\n")
    file.close()

def build_subline_index(fname_path,params):
    fixed_subline_length = 3 
    import ntpath
    fname_path = str(ntpath.normpath(fname_path.decode().rstrip()))
    fname = ntpath.basename(fname_path)

    min_fname_length = 500

    if len(fname) <= min_fname_length:
        min_fname_length = len(fname)

    temp_subline_length = fixed_subline_length

    if (temp_subline_length > min_fname_length):
        temp_subline_length = min_fname_length

    shift = temp_subline_length - 2
    i = 0
    while i != (fname.rindex(fname[-1])-shift):
        substr = fname[i:(i+temp_subline_length)]
        i+=1
        yield str(substr), str(fname_path)

def merge_map_sym(iter, params):
    from disco.util import kvgroup
    for it1, it2 in iter:
        yield it1, str(it2)

def merge_map_sub(iter, params):
    for it1, it2 in iter:
        yield it1, str(it2)

if __name__ == '__main__':
    sizes = []
    execution_time = []
    input = ""
    output = ""
    metrics = ""
    cnt = 0
    length = 0
    if len(sys.argv) == 7:
        for argv in sys.argv:

            if (argv == "--input"):
                input = sys.argv[cnt + 1]
            if (argv == "--metrics"):
                metrics = sys.argv[cnt + 1]
            if (argv == "--output"):
                output = sys.argv[cnt + 1]
            cnt += 1
    else:
        print("enter correct parameters\n")
        print("example: python computeInDisco.py --input test-tag --metrics make_metrics.csv --output result-file")
        exit(1)

    print('\n\n',input, output, metrics)

    ddfs_connection = disco.ddfs.DDFS("disco://localhost")

    tags_list = ddfs_connection.list(input)

    sizes = []
    for tag in tags_list:  # Cutting fname and extention tag02_txt... -> 02
        sizes.append(re.sub("{}".format(input), "", tag).rsplit("_", 1)[0])

    print("\n\nTAGS", tags_list);
    print("\n\nSIZES", sizes);

    str = ""
    for tag, size in zip(tags_list,sizes): 

        input_file = []

        if (len(ddfs_connection.urls(tag)[0]) > 3):
            it = 0
            while cnt != 0:
                cnt = len(ddfs_connection.urls(tag)[0]) // 3
                input_file.append(ddfs_connection.urls(tag)[0][0+it])
                it += 3
        else :
            input_file.append(ddfs_connection.urls(tag)[0][0])

        print("\n\ninput_file", input_file);

        begin = time.time()

        job_1 = Job().run(input=input_file, map=build_subline_index, reduce=merge_map_sub)
        job_2 = Job().run(input=input_file, map=build_symbol_index, reduce=merge_map_sym)
        res_job_1 = job_1.wait()
        res_job_2 = job_2.wait()


        end = time.time()
        print_index(res_job_1,"subline-" + output + size + ".txt")
        print_index(res_job_2,"symbol-" + output + size + ".txt")

        elapsed_ms = (end - begin)*1000
        print("--- %s milliseconds ---" % elapsed_ms)
        execution_time.append(elapsed_ms)


    count_volume_list = []
    for it1, it2 in zip(sizes, execution_time):
        count_volume_list.append((int(it1), it2))

    count_volume_list.sort(key=lambda tup: tup[0])

    file_csv = open(metrics, "w")
    file_csv.write("Count of data;Processing time(ms)\n")
    for it1, it2 in count_volume_list:
        file_csv.write(it1.__str__() + ";" + it2.__str__() + "\n")
    file_csv.close()
