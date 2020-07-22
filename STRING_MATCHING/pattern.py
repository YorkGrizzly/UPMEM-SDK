import random

vocabulary = ['A', 'T', 'C', 'G']
LENGTH = 16777216
QUERY = "AACCTGAG"

with open("corpus.txt", "w+") as w_corpus_file:
    match = 0
    # corpus = ""
    # marker = []
    for i in range(LENGTH):
        w_corpus_file.write(random.choice(vocabulary))
# print(corpus)

    w_corpus_file.seek(0)
    r_corpus_file = w_corpus_file.read()

    with open("golden_ans.txt", "w") as golden_file:
        for pos, token in enumerate(r_corpus_file):
            match = match + 1 if token == QUERY[match] else 0
            if match == len(QUERY):
                golden_file.write(str(pos - len(QUERY) + 1) + "\n")
                match = 0

    golden_file.close()
    w_corpus_file.close()

# print(marker)
# for item in marker:
#     print(corpus[item:item+len(QUERY)])



 
