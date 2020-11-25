"""
Точка входа для программы, решающей задачу лабораторной работы № 4
"""
import sys
import disco.ddfs
import glob
import re
import time

if __name__ == '__main__':
    cnt = 0
    input = ""
    tag_name = ""
    metrics = ""
    if len(sys.argv) == 7:
        for argv in sys.argv:
            # print(argv)
            if (argv == "--input"):
                input = sys.argv[cnt + 1]
            if (argv == "--tag-name"):
                tag_name = sys.argv[cnt + 1]
            if (argv == "–-metrics"):
                metrics = sys.argv[cnt + 1]
            cnt += 1
    else:
        print("enter correct parameters\n")
        print("example: python load2DDFS.py --input inputFilesName --tag-name tagName –-metrics metrics.csv")
        exit(1)

    print('\n\nINPUT ARGS : ',input, tag_name, metrics)

    #MAKE CONNECTION TO DDFS
    ddfs_connection = disco.ddfs.DDFS("disco://localhost")



    inputs_list = glob.glob(input+"*.txt")
    print("\n\nINPUT FILES:\n ",inputs_list)

    sizes = []
    for fname in inputs_list:  # Cutting fname and extention file02.txt -> 02
        sizes.append(re.sub("{}".format(input), "", fname).rsplit(".", 1)[0])

    print("\n\nSIZES FILES:\n ", sizes)

    execution_time = []

    for it1, it2 in zip(inputs_list, sizes):
        begin = time.time()
        ddfs_connection.push(tag_name + it2, [it1])
        end = time.time()
        execution_time.append((end - begin) * 1000)

    count_volume_list = []
    for it1, it2 in zip(sizes, execution_time):
        count_volume_list.append((int(it1), it2))

    count_volume_list.sort(key=lambda tup: tup[0])

    file_csv = open(metrics, "w")
    file_csv.write("Count of data;Download time(ms)\n")
    for it1, it2 in count_volume_list:
        file_csv.write(it1.__str__() + ";" + it2.__str__() + "\n")
    file_csv.close()

print("\n\n* Computing on virtual Disco cluster will be started soon ...")

#GET COMMANDS
    #print('\n\nGET tag-object INFO BY TAG\n',ddfs_connection.get("tag"))
    #print('\n\nGET attributes-list INFO BY TAG',ddfs_connection.attrs("tag"))
    #print('\n\nGET blobs INFO BY TAG\n',ddfs_connection.blobs("tag"))
    #print('\n\nGET attributes-list INFO BY TAG',ddfs_connection.attrs("tag"))
    #print('\n\nGET blobs INFO BY TAG\n',ddfs_connection.blobs("tag"))
    #print('\n\nGET value of attribute\n',ddfs_connection.getattr("data:bigtxt","some_attr"))
    #print('\n\nGET list of tags starts with prefix\n',ddfs_connection.list(""))
    #print('\n\nGET URLs BY TAG\n',ddfs_connection.urls("tag"))
    #print('\n\nGET tag graph starting at TAG\n',ddfs_connection.walk("tag"))
    #print('\n\nCHECK tag if-exists BY TAG\n',ddfs_connection.blobs("tag"))

    #MODIFY COMMANDS
    ##print('\n\nAPPLY blobfilter for tag\n',ddfs_connection.pull("data:bigtxt"))
    ##print('\n\nPUSH list of files WITH TAG\n',ddfs_connection.push("data:bigtxt",
        #[("./bigfile.txt","name1"),("./bigfile2.txt","name2")]))
    ##print('\n\nADD LIST OF URLs to TAG\n',ddfs_connection.put("data:bigtxt",["url1","url2"]))
    ##print('\n\nAPPEND LIST OF URLs to TAG\n',ddfs_connection.tag("data:bigtxt",["url1","url2"]))
    ##print('\n\nSET value of attribute\n',ddfs_connection.setattr("data:bigtxt","some_attr","value"))
    ##print('\n\nSPLIT url to CHUNKS, push it with TAG\n',ddfs_connection.chunk("data:bigtxt",["url1","url2"]))

    #DELETE COMMANDS
    ##print('\n\nDELETE attribute\n',ddfs_connection.delattr("data:bigtxt","some_attr"))
    ##print('\n\nDELETE tag\n',ddfs_connection.delete("data:bigtxt")))

    ##print('\n\nDELETE first tag in the list\n',ddfs_connection.delete(ddfs_connection.list("")))