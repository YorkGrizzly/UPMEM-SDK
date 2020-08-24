import os
import sys
import subprocess

for i in range(100):
    print("\nTestcase " + str(i) + ":")
    QUERY = subprocess.getoutput("python3 pattern.py")
    print("Using QUERY", QUERY)
    # os.system("python3 pattern.py")
    gold_ans_cnt = subprocess.getoutput("cat golden_ans.txt | wc -l")
    grep_ans_cnt = subprocess.getoutput("grep -aob './corpus.txt' -e {} | wc -l".format(QUERY))
    print("golden ans found {} matches".format(gold_ans_cnt))
    print("grep found {} matches".format(grep_ans_cnt))
    if gold_ans_cnt == grep_ans_cnt:
        print("The two match!")
    else:
        print("ERROR! NO MATCHING COUNT!!!")
        break
    if i == 99:
        print("SUCCESS! ALL TESTCASES MATCH!!!")


    # os.system("grep -aob './corpus.txt' -e 'AACCTGAG' | wc -l")
