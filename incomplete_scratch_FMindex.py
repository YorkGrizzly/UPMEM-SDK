'''
ranks indicates the occurences of that character in the string up to that point
give rankings after sorting roatations lexicographically
ranks can be inferred implicitly


1. rotate and sort string
2. extract F and L column
3. build tally table (nb of checkpoints * nb of alphabets)
4. implement rank calculation: go to neareset checkpoint, counting occurences along the way and infer the rank
5. build suffix array (# of checkpoints), save checkpoints as constant offsets apart
6. calculate offsets: use LF mapping to reach a row which has a checkpointed occurence count, add number of steps used to reach that row to deduce rank of non-checkpointed row 

T: m characters

need F column: number of integers
need L column: number of characters (same as T string)
SA sample: m*fraction of rows kept
checkpoints: m*alphabet size*fraction of rows checkpointed
'''

corpus = "ataattcccgg$" # $ indicates the end of a string

alphabet = ('A', 'T', 'C', 'G')

class FMindex():
    def __init__(self, corpus, tally_ratio=1/len(corpus), SA_ratio=1/len(corpus), step=1):
        self.corpus = corpus
        self.corpus_size = len(corpus)
        self.tally_ratio = tally_ratio
        self.SA_ratio = SA_ratio
        self.step = step
        self.F_col, self.L_col, self.SA = self.extract_L_F_SA()
        self.a_cnt, self.t_cnt, self.c_cnt, self.g_cnt, self.occ_a, self.occ_t, self.occ_c, self.occ_g = self.build_tally()

    def rotate(self):
        rotations = []
        corpus_size_offset = self.corpus_size - 1
        # calculate SA for each rotation
        for i in range(self.corpus_size):
            self.corpus = self.corpus[-1] + self.corpus[:-1]
            rotations.append((corpus_size_offset - i, self.corpus))

        print(rotations)
        # take SA ratio
        SA_chckpt = int(self.corpus_size*self.SA_ratio)
        print(SA_chckpt)
        for i in range(len(rotations)):
            if i % SA_chckpt != 0:
                rotations[i] = (-1, rotations[i][1])

        print(rotations)
        # sort rotations
        rotations.sort(key=lambda pair: pair[1])
        print(rotations)
        return rotations

    def extract_L_F_SA(self):
        rotations = self.rotate()
        F_col = []
        L_col = []
        SA = []
        for rot in rotations:
            F_col.append(rot[1][0])
            L_col.append(rot[1][-1])
            if rot[0] != -1:
                SA.append(rot[0])
            else:
                SA.append(None)

        # print(rotations)
        # print(F_col)
        # print(L_col) 
        return F_col, L_col, SA




    def build_tally(self):
        occ_a = []
        occ_t = []
        occ_c = []
        occ_g = []

        a_cnt = 0
        t_cnt = 0
        c_cnt = 0
        g_cnt = 0
        
        check_step = int(len(self.L_col)*self.tally_ratio)

        for i, token in enumerate(self.L_col):
            if token == 'a':
                a_cnt += 1
            elif token == 't':
                t_cnt += 1
            elif token == 'c':
                c_cnt += 1
            elif token == 'g':
                g_cnt += 1
            elif token != '$':
                print("detected unrecognized character: " + str(token))
            
            if i % check_step == 0:
                occ_a.append(a_cnt)
                occ_t.append(t_cnt)
                occ_c.append(c_cnt)
                occ_g.append(g_cnt)

        return a_cnt, t_cnt, c_cnt, g_cnt, \
            occ_a, occ_t, occ_c, occ_g

    def display(self):
        print('L  ' + 'OCC_A  ' + 'OCC_T  ' + 'OCC_C  ' + 'OCC_G  ' + "SA")
        occ_check_step = int(self.corpus_size * self.tally_ratio)
        for i, char in enumerate(self.L_col):
            if i % occ_check_step == 0:
                k = int(i / occ_check_step)
                print(char + '     ' + str(self.occ_a[k]) + '     ' + str(self.occ_t[k]) + '     ' + str(self.occ_c[k]) + '     ' + str(self.occ_g[k]), end='      ')
            else:
                print(char, end="                              ")
            print(self.SA[i])


        



fmindex = FMindex(corpus, tally_ratio=1/4, SA_ratio=1/3)
print(fmindex.L_col)
print(fmindex.SA)
# print(fmindex.a_cnt, fmindex.t_cnt, fmindex.c_cnt, fmindex.g_cnt)
# print(fmindex.occ_a. fmindex.occ_b)

fmindex.display()

# print(fmindex.L_col)