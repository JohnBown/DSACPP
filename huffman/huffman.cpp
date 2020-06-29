
#define N_CHAR (0x80 - 0x20) //仅以可打印字符为例
struct HuffChar {            // Huffman(超)字符
    char ch;                 //字符
    int weight;              //频率
    HuffChar(char c = '^', int w = 0) : ch(c), weight(w) {};
    bool operator<(HuffChar const& hc) { return weight > hc.weight; }   //比较器(此处故意大小颠倒)
    bool operator==(HuffChar const& hc) { return weight == hc.weight; } //判等器
};

#include "../bintree/binTree.h"    //由BinTree实现
#define HuffTree BinTree<HuffChar> // Huffman树, 节点类型为HuffChar

#include "../list/list.h"           //用List实现
typedef List<HuffTree*> HuffForest; // Huffman森林

#include "../bitmap/bitmap.h" //基于Bitmap实现
typedef Bitmap HuffCode;      // Huffman二进制编码

#include "../hashtable/hashtable.h"       //用HashTable实现
typedef Hashtable<char, char*> HuffTable; // Huffman编码表

int* statistics(char* sample_text_file) { //统计字符出现频率
    int* freq = new int[N_CHAR]; //以下统计需随机访问, 故以数组记录各字符出现次数
    memset(freq, 0, sizeof(int) * N_CHAR); //清零
    FILE* fp = fopen(sample_text_file, "r");
    for (char ch; 0 < fscanf(fp, "%c", &ch);) { //逐个扫描文本文件中的每个字符
        if (ch >= 0x20) freq[ch - 0x20]++;      //累计对应的出现次数
    }
    fclose(fp);
    return freq;
}

HuffForest* initForest(int* freq) {         //根据频率统计表, 为每个字符创建一棵树
    HuffForest* forest = new HuffForest;    //以List实现的Huffman森林
    for (int i = 0; i < N_CHAR; i++) {      //为每个字符
        forest->insertAsLast(new HuffTree); //生成一颗树, 并将字符及其频率
        forest->last()->data->insertAsRoot(HuffChar(0x20 + i, freq[i])); //存入其中
    }
    return forest;
}

HuffTree* minHChar(HuffForest* forest) {               //在Huffman森林中找出权重最小的(超)字符
    ListNodePosi(HuffTree*) p       = forest->first(); //从首节点出发查找
    ListNodePosi(HuffTree*) minChar = p;               //最小的Huffman树所在的节点位置
    int minWeight                   = p->data->root()->data.weight; //目前的最小权重
    while (forest->valid(p = p->succ)) {                            //遍历所有节点
        if (minWeight > p->data->root()->data.weight) {             //若当前节点所含树更小, 则
            minWeight = p->data->root()->data.weight;               //更新记录
            minChar   = p;
        }
    }
    return forest->remove(minChar); //将挑选出的Huffman树从森林中摘除, 并返回
}

HuffTree* generateTree(HuffForest* forest) { // Huffman编码算法
    while (1 < forest->size()) {
        HuffTree* T1 = minHChar(forest);
        HuffTree* T2 = minHChar(forest);
        HuffTree* S  = new HuffTree();
        S->attachAsLC(S->root(), T1);
        S->attachAsRC(S->root(), T2);
        forest->insertAsLast(S);
    } //循环结束时, 森林中唯一(列表首节点)的那棵树即Huffman编码树
    return forest->first()->data;
}

static void generateCT(Bitmap* code, int length, HuffTable* table, BinNodePosi(HuffChar) v) {
    if (IsLeaf(*v)) { //若是叶节点
        table->put(v->data.ch, code->bits2string(length));
        return;
    }
    if (HasLChild(*v)) { // left = 0
        code->clear(length);
        generateCT(code, length + 1, table, v->lc);
    }
    if (HasRChild(*v)) { // right = 1
        code->set(length);
        generateCT(code, length + 1, table, v->rc);
    }
}

HuffTable* generateTable(HuffTree* tree) { //将各字符编码统一存入以散列表实现的编码表中
    HuffTable* table = new HuffTable;
    Bitmap* code     = new Bitmap;
    generateCT(code, 0, table, tree->root());
    if (code) delete code; // release(code)
    return table;
}

int encode(HuffTable* table, Bitmap* codeString, char* s) { // PFC编码算法
    int n = 0;
    for (size_t m = strlen(s), i = 0; i < m; i++) {               //对于明文s[]中的每个字符
        char** pCharCode = table->get(s[i]);                      //取出其对应的编码串
        if (!pCharCode) pCharCode = table->get(s[i] + 'A' - 'a'); //小写字母转为大写
        if (!pCharCode) pCharCode = table->get(' ');              //无法识别的字符统一视作空格
        printf("%s", *pCharCode);                                 ///输出当前字符的编码
        for (size_t m = strlen(*pCharCode), j = 0; j < m; j++) {  //将当前字符的编码接入编码串
            '1' == *(*pCharCode + j) ? codeString->set(n++) : codeString->clear(n++);
        }
    }
    printf("\n");
    return n; //二进制编码串记录于codeString中, 返回编码串总长
}

void decode(HuffTree* tree, Bitmap* code, int n) { // PFC解码算法
    BinNodePosi(HuffChar) x = tree->root();        //根据PFC编码树
    for (int i = 0; i < n; i++) {                  //将编码(二进制位图)
        x = code->test(i) ? x->rc : x->lc;         //转译为明码并
        if (IsLeaf(*x)) {
            printf("%c", x->data.ch); //打印输出
            x = tree->root();
        }
    }
}

int main(int argc, char* argv[]) {            // Huffman编码算法统一测试
    int* freq          = statistics(argv[1]); //根据样本文件, 统计各字符的出现频率
    HuffForest* forest = initForest(freq);    //创建Huffman森林
    if (freq) delete freq;                    // release(freq)
    HuffTree* tree = generateTree(forest);    //生成Huffman编码树
    if (forest) delete forest;                // release(forest)
    HuffTable* table = generateTable(tree);   //将Huffman编码树转换为编码表
    for (int i = 2; i < argc; i++) {          //对于命令行传入的每一明文串
        Bitmap* codeString = new Bitmap;      //二进制编码串
        int n              = encode(table, codeString, argv[i]); //将根据编码表生成(长度为n)
        decode(tree, codeString, n);       //利用Huffman编码树, 对长度为n对二进制编码串解码
        if (codeString) delete codeString; // release(codeString)
    }
    if (table) delete table; // release(table)
}