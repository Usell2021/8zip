#include <bitset>
#include <complex>
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

    for (int i = 0; i < 9; i++) {
        std::cout << tree[i].symbol << ' ' << tree[i].left << ' ' << tree[i].right << ' ' << tree[i].parent << '\n';
    }

    getKeys(tree, tree_size - 1, 0, 0);

    for (int i = 0; i < 256; i++) {
        auto item = keys[i];
        if (item.second != 0)
            std::cout << char(i) << ' ' << int(item.first) << ' ' << int(item.second) << '\n';
    }

    inputFile = fopen("./input.txt", "rb");
    auto outputFile = fopen("./output.txt", "w+");
    char byte = 0, lenght = 0;
    char previousByte = 0;
    while (fscanf(inputFile, "%c", &ch) != -1) {
        char code = keys[ch].first;
        char mask = keys[ch].second;
        while (mask & 1) {
            if (lenght == 8) {
                fprintf(outputFile, "%c", byte);
                std::cout << std::bitset<8>(byte);
                previousByte = byte;
                byte = 0, lenght = 0;
            }
            byte <<= 1;
            if (code & 1) {
                byte++;
            }
            mask >>= 1;
            code >>= 1;
            lenght++;
        }
    }
    if (lenght > 0) {
        fprintf(outputFile, "%c", previousByte);
        std::cout << std::bitset<8>(previousByte);

    }

    std::cout << "lenght:\n" << int(lenght) << "\n";

    auto test = fopen("./output.txt", "rb");
    while (fscanf(test, "%c", &ch) != -1) {
    }

    std::cout << "\nnorm";
}
