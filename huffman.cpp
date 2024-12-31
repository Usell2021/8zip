#include <bitset>
#include <iostream>
#include <string.h>
//#include <filesystem>

typedef unsigned long long ull;

struct Forest {
    ull weight = 0;
    int root = 0;
    unsigned char symbol = 0;
};

struct Tree {
    int left = -1;
    int right = -1;
    int parent = -1;
    unsigned char symbol = 0;
};

ull frequency[256];
Forest forest[256];
Tree tree[512];
int tree_size;
int forest_size;
int frequency_size;
std::pair<ull, ull> keys[256];

std::pair<int, int> getMinsIdx() {
    Forest wm = {
        .weight = __LONG_LONG_MAX__,
        .root = 0,
    };
    Forest a = wm, b = wm;

    for (int i = 0; i < forest_size; i++) {
        if (a.weight > forest[i].weight) {
            b = a;
            a = {
                .weight = forest[i].weight,
                .root = i
            };
        } else if (b.weight > forest[i].weight && a.root != i) {
            b = {
                .weight = forest[i].weight,
                .root = i
            };
        }
    }
    return {a.root, b.root};
}


void getKeys(int p, ull k, ull m) {
    if (tree[p].left == tree[p].right) {
        keys[tree[p].symbol].first = k;
        if (m == 0)
            m++;
        keys[tree[p].symbol].second = m;
    } else if (tree[p].left != -1) {
        getKeys(tree[p].left, k * 2, m * 2 + 1);
    }
    if (tree[p].right != -1) {
        getKeys(tree[p].right, k * 2 + 1, m * 2 + 1);
    }
}

void buildForest() {
    for (int i = 0; i < 256; i++) {
        auto item = frequency[i];
        if (item > 0) {
            forest[forest_size] = {
                .weight = item,
                .root = forest_size,
                .symbol = i,
            };

            forest_size++;
        }
    }
}

void buildTree() {
    tree_size = forest_size;
    if (forest_size == 1) {
        tree[0] = {
            .parent = 1,
            .symbol = forest[0].symbol
        };
        tree[1] = {
            .left = 0,
        };
        tree_size = 2;
    }

    while (forest_size > 1) {
        auto mins = getMinsIdx();
        auto fa = forest[mins.first], fb = forest[mins.second];
        tree_size++;

        int tr = tree_size - 1;
        Forest ft = {
            .weight = fa.weight + fb.weight,
            .root = tr
        };

        tree[fa.root] = {
            .left = tree[fa.root].left,
            .right = tree[fa.root].right,
            .parent = tr,
            .symbol = fa.symbol,
        };
        tree[fb.root] = {
            .left = tree[fb.root].left,
            .right = tree[fb.root].right,
            .parent = tr,
            .symbol = fb.symbol,
        };
        tree[tr] = {
            .left = fa.root,
            .right = fb.root,
            .parent = tree[tr].parent
        };


        if (mins.first == forest_size - 1) {
            forest[mins.second] = ft;
        } else if (mins.second == forest_size - 1) {
            forest[mins.first] = ft;
        } else {
            forest[mins.first] = ft;
            forest[mins.second] = forest[forest_size - 1];
        }

        forest_size--;
    }
}

void writeData(FILE *archiveFile, FILE *inputFile) {
    unsigned char byte = 0, length = 0, ch;
    rewind(inputFile);
    while (fscanf(inputFile, "%c", &ch) != -1) {
        ull code = keys[ch].first;
        ull mask = keys[ch].second;
        ull lastbit = 1;
        lastbit <<= 63;

        for (int i = 0; i < 64; i++) {
            if (mask & lastbit) {
                if (length == 8) {
                    fprintf(archiveFile, "%c", byte);
                    byte = 0, length = 0;
                }
                byte <<= 1;
                if (code & lastbit) {
                    byte++;
                }
                length++;
            }
            mask <<= 1;
            code <<= 1;
        }
    }
    if (length) {
        fprintf(archiveFile, "%c", byte);
    }

    rewind(archiveFile);
    fprintf(archiveFile, "%c", length); // write last byte length
}

void getFrequencies(FILE *inputFile) {
    unsigned char ch;
    while (fscanf(inputFile, "%c", &ch) != -1) {
        if (frequency[ch] == 0)
            frequency_size++;
        frequency[ch]++;
    }
}


ull getFileSize(FILE *archiveFile) {
    fseek(archiveFile, 0, SEEK_END);
    ull fileSize = ftell(archiveFile);
    rewind(archiveFile);
    return fileSize;
}

void encodeData(FILE *archiveFile, FILE *outputFile, ull fileSize, unsigned char lastByteLen) {
    auto node = tree[tree_size - 1];
    unsigned char ch;
    ull curByteLen = 8;
    while (fscanf(archiveFile, "%c", &ch) != -1) {
        long curPos = ftell(archiveFile);
        if (curPos == fileSize) {
            curByteLen = lastByteLen;
            ch <<= 8 - curByteLen;
        } else
            curByteLen = 8;
        while (curByteLen != 0) {
            if (node.right == node.left) {
                fprintf(outputFile, "%c", node.symbol);
                node = tree[tree_size - 1];
            }
            if (ch & 128) {
                node = tree[node.right];
            } else {
                node = tree[node.left];
            }
            ch <<= 1;
            curByteLen--;
        }
    }
    fprintf(outputFile, "%c", node.symbol);
}

void writeFrequency(FILE *archiveFile) {
    fprintf(archiveFile, "%c\n", 0); // reserve byte for last byte length
    fprintf(archiveFile, "%c\n", frequency_size);
    for (int i = 0; i < 256; ++i) {
        if (frequency[i]) {
            fprintf(archiveFile, "%d %llu\n", i, frequency[i]);
        }
    }
}

void readFrequency(FILE *archiveFile) {
    unsigned char ch;
    ull f;
    if (frequency_size == 0)
        frequency_size = 256;
    for (int i = 0; i < frequency_size; ++i) {
        fscanf(archiveFile, "%d %llu\n", &ch, &f);
        frequency[ch] = f;
    }
}

void archive(char archiveName[], char fileName[]) {
    auto inputFile = fopen(fileName, "rb");
    getFrequencies(inputFile);

    buildForest();
    buildTree();

    getKeys(tree_size - 1, 0, 0);

    auto archiveFile = fopen(archiveName, "wb");

    writeFrequency(archiveFile);
    writeData(archiveFile, inputFile);
    fclose(inputFile);
    fclose(archiveFile);
}

void unarchive(char archiveName[], char fileName[]) {
    auto archiveFile = fopen(archiveName, "rb");

    ull fileSize = getFileSize(archiveFile);

    // get last byte length
    unsigned char lastByteLen;
    fscanf(archiveFile, "%c\n", &lastByteLen);
    fscanf(archiveFile, "%c\n", &frequency_size);
    fseek(archiveFile, -1, SEEK_CUR);

    readFrequency(archiveFile);

    buildForest();
    buildTree();

    auto outputFile = fopen(fileName, "wb");
    encodeData(archiveFile, outputFile, fileSize, lastByteLen);

    fclose(archiveFile);
    fclose(outputFile);
}

int main(int argc, char *argv[]) {
    if (argc == 4) {
        auto command = argv[1];
        auto archName = argv[2];
        auto fileName = argv[3];
        if (!strcmp("encode", command)) {
            /*if (std::filesystem::exists(archName)) {
                printf("Archive with this name exist. Rewrite? [y/n]");
                char ch;
                scanf("%c", &ch);
                if (ch == 'n')
                    return 0;
                remove(archName);
            }
            if (!std::filesystem::exists(fileName)) {
                printf("File with this name does not exist.");
                return 0;
            }*/

            archive(archName, fileName);
            std::cout << "Archived\n";
        } else if (!strcmp("decode", command)) {
            /*if (!std::filesystem::exists(archName)) {
                printf("Archive with this name does not exist.");
                return 0;
            }
            if (std::filesystem::exists(fileName)) {
                printf("File with this name exists. Rewrite? [y/n]");
                char ch;
                scanf("%c", &ch);
                if (ch == 'n')
                    return 0;
                remove(fileName);
            }*/

            unarchive(archName, fileName);
            std::cout << "Unarchived\n";
        }
    } else
        printf("Unknown command.");
}
