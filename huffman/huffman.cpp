#include <bitset>
#include <iostream>

FILE *inputFile;

struct Forest {
    int weight;
    int root;
    char symbol;
};

struct Tree {
    int left;
    int right;
    int parent;
    char symbol;
};

std::pair<int, int> getMinsIdx(Forest f[], int s) {
    Forest wm = {
            .weight = 0,
            .root = 0,
    };

    for (int i = 0; i < s; i++) {
        if (wm.weight < f[i].weight)
            wm = {
                    .weight = f[i].weight,
                    .root = i
            };
    }
    Forest a = wm, b = wm;

    for (int i = 0; i < s; i++) {
        if (a.weight > f[i].weight) {
            a = {
                    .weight = f[i].weight,
                    .root = i
            };
        }
    }

    for (int i = 0; i < s; i++) {
        if (f[i].weight <= b.weight && a.root != i) {
            b = {
                    .weight = f[i].weight,
                    .root = i
            };
        }
    }
    return {a.root, b.root};
}


int frequency[256];
Forest forest[256];
Tree tree[512];
int tree_size;
int forest_size;
std::pair<char, char> keys[256];

void getKeys(Tree t[], int p, char k, char l) {
    if (t[p].left == t[p].right) {
        keys[tree[p].symbol].first = k;
        keys[tree[p].symbol].second = l;
    } else if (t[p].left != -1) {
        getKeys(t, t[p].left, k * 2, l * 2 + 1);
    }
    if (t[p].right != -1) {
        getKeys(t, t[p].right, k * 2 + 1, l * 2 + 1);
    }
}

int main() {
    inputFile = fopen("./input.txt", "rb");
    unsigned char ch;

    while (fscanf(inputFile, "%c", &ch) != -1) {
        frequency[ch]++;
    }

    for (int i = 0; i < 256; i++) {
        auto item = frequency[i];
        if (item > 0) {
            forest[forest_size] = {
                    .weight = item,
                    .root = forest_size,
                    .symbol = char(i),
            };

            forest_size++;
        }
    }

    for (auto &item: tree) {
        item = {
                .left = -1,
                .right = -1,
                .parent = -1,
                .symbol = -1,
        };
    }

    tree_size = forest_size;
    while (forest_size > 1) {
        auto mins = getMinsIdx(forest, forest_size);
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

    getKeys(tree, tree_size - 1, 0, 0);

    rewind(inputFile);
    auto outputFile = fopen("./output.txt", "w+");
    char byte = 0, length = 0;
    while (fscanf(inputFile, "%c", &ch) != -1) {
        char code = keys[ch].first;
        char mask = keys[ch].second;
        for (int i = 0; i < 8; i++) {
            if (mask & 128) {
                if (length == 8) {
                    fprintf(outputFile, "%c", byte);
                    byte = 0, length = 0;
                }
                byte <<= 1;
                if (code & 128) {
                    byte++;
                }
                length++;
            }
            mask <<= 1;
            code <<= 1;
        }
    }
    if (length > 0) {
        fprintf(outputFile, "%c", byte);
    }
    fclose(outputFile);

    auto output = fopen("./output.txt", "rb");\

    auto knot = tree[tree_size - 1];
    int len = 8;
    fseek(output, 0, SEEK_END);
    long file_size = ftell(output);
    rewind(output);

    while (fscanf(output, "%c", &ch) != -1) {
        long currect_pos = ftell(output);
        if (currect_pos == file_size) {
            len = length;
            ch <<= 8 - len;
        } else
            len = 8;
        while (len != 0) {
            if (knot.right == knot.left) {
                std::cout << knot.symbol;
                knot = tree[tree_size - 1];
            }
            if (ch & 128) {
                knot = tree[knot.right];
            } else {
                knot = tree[knot.left];
            }
            ch <<= 1;
            len--;
        }
    }
    std::cout << knot.symbol;


    std::cout << "\nnorm";
}
