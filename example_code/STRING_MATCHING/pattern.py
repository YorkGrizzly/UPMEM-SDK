import random

vocabulary = ['A', 'T', 'C', 'G']
LENGTH = 262144
# QUERY = "AACCTGAG"
QUERY = ""

# generate random QUERY
# QUERY_LENGTH = random.randint(3, 10)
QUERY_LENGTH = 8
for i in range(QUERY_LENGTH):
    QUERY += random.choice(vocabulary)
print(QUERY)

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
        pos = 0
        while pos < LENGTH:
            token = r_corpus_file[pos]
            if token == QUERY[match]:
                match += 1
            else:
                pos -= match
                match = 0
            # match = match + 1 if token == QUERY[match] else 0
            if match == len(QUERY):
                golden_file.write(str(pos - len(QUERY) + 1) + "\n")
                match = 0
            pos += 1

    golden_file.close()
    w_corpus_file.close()

# print(marker)
# for item in marker:
#     print(corpus[item:item+len(QUERY)])



 
