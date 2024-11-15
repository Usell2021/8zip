#include <iostream>

FILE *f;

struct Forest {
    int weight;
    int root;
};

struct Tree {
    int left;
    int right;
    int parent;
};

int frequency[256];
Forest forest[256];
Tree tree[512];
int tree_size;
int forest_size;

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
        if (a.weight < f[i].weight && f[i].weight < b.weight) {
            b = {
                    .weight = f[i].weight,
                    .root = i
            };
        }
    }
    return {a.root, b.root};
}

int main() {
    f = fopen("./input.txt", "rb");
    unsigned char ch;

    while (fscanf(f, "%c", &ch) != -1) {
        frequency[ch]++;
    }

    int i = 0;
    for (const auto &item: frequency) {
        if (item > 0) {
            forest[i] = {
                    .weight = item,
                    .root = i
            };

            i++;
            forest_size++;
        }
    }

    for (auto &item: tree) {
        item = {
                .left = -1,
                .right = -1,
                .parent = -1,
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
                .parent = tr
        };
        tree[fb.root] = {
                .left = tree[fb.root].left,
                .right = tree[fb.root].right,
                .parent = tr
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
        std::cout << tree[i].left << ' ' << tree[i].right << ' ' << tree[i].parent << '\n';
    }
}