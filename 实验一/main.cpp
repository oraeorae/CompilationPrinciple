/**
 * @author czh
 * @date 2022.9.25
 * @descrion 3.0版本（结合switch和前缀树）
 * 改进：1.文件读入读出，原本读出一个完整的字符串再写入文件，现在改成每个字符以二进制形式依次写入
 * 已解决Bug：1.字符串中有中文会引起错误（已解决）
 *          2.还未解决/**的注释（已解决）
 *          3.读取/=或者/不行（已解决）
 *          4.关键词中有符号（已解决）
 * Bug：
 *      for循环的也会换行
 */

# include<iostream>
# include<string>
# include<vector>
# include<algorithm>
# include<fstream>
# include<ctype.h>
# include<stack>
# include<unordered_map>
#include <sstream>

using namespace std;
const string POS = "D:\\code\\cpp\\byyl\\";
/**
 * 前缀树
 */
struct Node{
    bool is_end;        //结束的标记
    int end_index;      //标记该字符
    unordered_map<char,Node*> son;      //26个字母+1个下划线
    Node(){
        is_end = false;
        end_index = -1;
    }
}*root = new Node();

/**
 * 前缀树的插入函数，即将单词存储到前缀树中
 * @param word
 * @param index
 * @param root
 */
void insert(string word,int index,Node* root) {
    auto p = root;
    for( auto c : word ){
        //不存在就创建
        if( !p->son[c] ) p->son[c] = new Node();
        p = p->son[c];
    }
    p->is_end = true;
    p->end_index = index;
}

/**
 * 前缀树的搜索函数，搜索树中是否有该单词
 * @param word
 * @param index
 * @param root
 * @return
 */
bool search(string word,int& index,Node* root) {
    auto p = root;
    for( auto c : word ){
        if( !p->son[c] ){
            index = -1;
            return false;
        }
        p = p->son[c];
    }
    index = p->end_index;
    return p->is_end;
}
/**
 * 前缀树的判断是否有前缀函数，搜索树中是否有该搜索字作为单词的前缀
 * @param word
 * @param root
 * @return
 */
bool startWith(string word,Node* root){
    auto p = root;
    for( auto c : word ){
        if( !p->son[c] ){
            return false;
        }
        p = p->son[c];
    }
    return true;
}

//索引对应的关键词
string keyword[90];

/**
 * @describe 将文件中的关键字读到字典树中
 * @param pos 文件位置
 * @param trie 字典树
 */
void fileToTrie(string pos){
    fstream iof;
    iof.open(pos,ios::in);		//在这里是读
    if (!iof.is_open())
    {
        cout << "文件不存在！" << endl;
        iof.close();
        return;
    }
    string str;
    int i = 1;
    while(!iof.eof()){
        iof>>str;
        if( iof.fail() )    break; //避免结尾的最后一次再读
        insert(str,i,root);
        //trie->insert(str,i);
        keyword[i] = str;
        i++;
    }
    iof.close();
}

/**
 * @describe 将文件中的代码读到字符串中
 * @param pos 文件位置
 * @param code
 */
void fileToCode(string pos,string& code){
    fstream iof;
    iof.open(pos,ios::in);		//在这里是读
    if (!iof.is_open())
    {
        cout << "文件不存在！" << endl;
        iof.close();
        return;
    }
    string str;
    while(!iof.eof()){
        getline(iof,str);       //获取一行
        if( iof.fail() )    break; //避免结尾的最后一次再读
        code += str + "\n";
    }
    iof.close();
}

/**
 * @describe 将压缩的代码写入文件中
 * @param pos 文件位置
 * @param a 写入的字符
 */
void CodeToFile(string pos,unsigned char a){
    fstream iof;
    iof.open(pos + "compass.txt",ios::app|ios::binary);  		//out这里是写入
    //写入内容
    iof.write((const char *)(&a), sizeof(a));
    iof.close();
}

/**
 * @describe 将压缩的代码写入文件中
 * @param pos 文件位置
 * @param a 写入的字符
 */
void CodeToFile(string pos,string word){
    fstream iof;
    iof.open(pos + "compass.txt",ios::app|ios::binary);  		//out这里是写入
    //写入内容
    for( unsigned char a : word ){
        iof.write((const char *)(&a), sizeof(a));
    }
    iof.close();
}

/**
 * @describe 删除文件最后一个字符
 * @param pos 文件位置
 * @param a 写入的字符
 */
void CodeFilePop(string pos){
    fstream iof;
    //写入内容
    stringstream buffer;
    iof.open(pos + "compass.txt",ios::in);
    buffer << iof.rdbuf();
    string contents = buffer.str();
    iof.close();
    contents.pop_back();
    iof.open( pos + "compass.txt" , ios::out|ios::trunc);
    iof << contents;
    iof.close();
}

/**
 * @describe 读压缩文本最后一个字符
 * @param pos 文件位置
 * @param a 写入的字符
 */
unsigned char CodeFileBack(string pos){
    fstream iof;
    //读内容
    iof.open(pos + "compass.txt",std::ios::in|ios::binary);
    //从倒数第一个字符开始写
    iof.seekp(-1,std::ios::end);
    unsigned char a;
    iof >> a;
    iof.close();
    return a;
}

/**
 * @describe 判断是否为关键词里面的符号
 * @param w
 * @return
 */
bool isKeywordCharacter(char w){

    if( w == '_' || w == '+' || w == '-' || w == '*' || w == '/' || w == '=' || w == '%' ){
        return true;
    }else{
        return false;
    }

}


/**
 * @describe 压缩字符方法
 * @param code
 */
void compassCode(string& code){
    string code2 = "";
    for( int i = 0 ; i < code.size() ;  ) {
        switch (code[i]) {
            case '"':           //遇到字符串则直接读，直到结束
            {
                string tmp = "";
                tmp += '"';
                i++;
                while (i < code.size() && code[i] != '"') {
                    tmp += code[i];
                    i++;
                }
                tmp += '"';
                //code2 += tmp;
                CodeToFile(POS, tmp);
                i++;
                break;
            }
            case 39:        //遇到单引号也直接读，直到结束
            {
                string tmp = "";
                tmp += char(39);
                i++;
                while (i < code.size() && code[i] != 39) {
                    tmp += code[i];
                    i++;
                }
                tmp += char(39);
                //code2 += tmp;
                CodeToFile(POS, tmp);
                i++;
                break;
            }
            case '\t':
            case '\n':
                //遇到缩进和换行，直接跳过
                i++;
                break;
            case ' ':           //遇到空格
                //if( !( code2.back() == ' ' || ispunct(code2.back())) ){     //如果code2结尾没有空格且不是符号，就加上空格
                if (!(CodeFileBack(POS) == ' ' || ispunct(CodeFileBack(POS)))) {     //如果code2结尾没有空格且不是符号，就加上空格
                    //code2 += code[i];
                    CodeToFile(POS, code[i]);
                }
                i++;
                break;
            case '/':           //遇到/，可能是单行注释或者多行注释,这里可能得考虑一下  /= 的情况
                i++;
                if (!(i < code.size())) {
                    break;
                }
                switch (code[i]) {
                    case '/':       //说明是“//”，单行注释，直接继续读直至遇到换行符
                        i++;
                        while (i < code.size() && code[i] != '\n') {
                            i++;
                        }
                        break;
                    case '*':      //说明是“/* */”，多行注释，继续读直到遇到“*/”
                        i++;
                        while (i + 1 < code.size() && (code[i] != '*' || code[i + 1] != '/')) {
                            i++;
                        }
                        i+=2;               //注意这里是+=2，否则会停留在 * 位置
                        break;
                    case '=':
                              int index;
                              if( search("/=",index,root) == true ){            //说明是前缀树里的单词，即关键字
                                  unsigned char a = 128 + index;
                                  CodeToFile(POS,a);
                              }
                              i++;
                              break;
                    default:
                        if (  CodeFileBack(POS) == 0 ) {        //如果当前是符号且code2结尾是空格，则去掉空格
                            CodeFilePop(POS);
                        }
                        CodeToFile(POS,'/');
                        break;
                }
                break;
            case '#':       //遇到#，说明是预编译，直接读到换行符
            {
                string tmp = "";
                while (i < code.size() && code[i] != '\n') {
                    if (code[i] != ' ') tmp += code[i];
                    i++;
                }
                CodeToFile(POS, tmp + "\n");         //头文件加上换行符
                i++;
                break;
            }
            default:
            {
                //关键字，用前缀树的搜索判断，如果是关键字，则替换，否则，直接写入
                {
                    string tmp = "";
                    int sign = true;            //标志是否为关键字
                    while( i < code.size() ){
                        tmp+=code[i];
                        if( startWith(tmp,root) == false ){         //如果关键字的前缀，则直接
                            sign = false;
                            break;
                        }
                        //解决   “i+”   直接读入的情况
                        if( isalpha(code[i]) && (ispunct(code[i+1]) && code[i+1] != '_' ) ){
                            sign = false;
                            break;
                        }
                        int index = -1;
                        if( search(tmp,index,root) == true ){           //如果是关键字,则读入文件
                            unsigned char a = 128 + index;
                            CodeToFile(POS, a);
                            tmp = "";
                            break;
                        }
                        i++;
                    }

                    if( sign == false ) {
                        //当只有一个符号时，明显没进入之前的关键词，所以不是关键词，直接写入
                        if( tmp.size() == 1 && ispunct(tmp[0] ) ){
                            if (  CodeFileBack(POS) == 0 ) {        //如果当前是符号且code2结尾是空格，则去掉空格
                                 CodeFilePop(POS);
                            }
                            CodeToFile(POS, tmp[0]);
                            tmp = "";
                            i++;
                            break;
                        }
                        //如果是符号并且不为下划线
                        if(ispunct(code[i]) && code[i]!='_'){
                            CodeToFile(POS, tmp);
                            tmp = "";
                            i++;
                            break;
                        }
                        //如果下个是字母或者下划线，则继续读
                        while( i < code.size() && (isalpha(code[i+1])  || code[i+1] == '_')  ){
                            i++;
                            tmp+=code[i];
                        }
                        CodeToFile(POS, tmp);
                        tmp = "";
                        i++;
                        break;
                    }

                    i++;
                    break;
                }
            }
        }
    }
}


/**
 * @describe 解压代码
 * @param code 要解压的代码
 * @param code2 解压成的代码
 */
void decompressCode(string pos,string &code2){

    int brackets = 0;
    fstream iof;
    //读内容
    iof.open(pos + "compass.txt",ios::in);

    char c;
    while(iof.get(c)){

        switch (c) {
            case '"':               //只需要处理双引号，因为中文字符也是超出ASCLL码，会引起下面判断，单引号可以不管
                code2 += c;
                iof.get(c);
                while( c != '"' ){
                    code2 += c;
                    iof.get(c);
                }
                code2 += c;
                break;
            case 39:               //处理单引号，避免出现‘}’的情况
                code2 += c;
                iof.get(c);
                while( c != 39 ){
                    code2 += c;
                    iof.get(c);
                }
                code2 += c;
                break;
            case ';':
            case '{':
            //case ':':
            case '}':
                //遇到以上4个，要换行
            {
                if (c == '{') {       //缩进+1
                    brackets++;
                } else if (c == '}') {        //缩进-1
                    code2.pop_back();           //原本的缩进再退一格
                    brackets--;
                }else if( c == ';' ){
                    if( code2.back() == '\n' ){             //解决“};”的情况
                        code2.pop_back();
                    }
                }
                code2 += c;
                code2 += '\n';
                int k = brackets;
                for( int i = 0 ; i < k && k > 0 ; i ++ ){
                    code2 += "\t";          //加缩进
                }
                break;
            }
            default:
                if( c <= 0 ){
                    code2 += keyword[c+128];
                }else{
                    code2 += c;
                }
                break;
        }
    }
    iof.close();
    return;
}
int main(){
    //创建文本（或者情况原本有的数据）
    fstream iof;
    iof.open( POS + "compass.txt" , ios::out|ios::trunc);
    iof.close();
    string code = "";
    //读取关键字到前缀树中
    fileToTrie("D:\\code\\cpp\\byyl\\keyword.txt");
    cout << "已成功导入关键词~" << endl;
    //读取想要压缩的代码
    fileToCode("D:\\code\\cpp\\byyl\\code.txt",code);
    cout << "原来的代码是：" << endl << code << endl;
    //进行压缩，并存储到code中
    compassCode(code);
    string tmp = "";
    fileToCode("D:\\code\\cpp\\byyl\\compass.txt",tmp);
    cout << "压缩后的代码是：" << endl << tmp << endl;
    string code2 = "";
    //进行解压
    decompressCode(POS,code2);
    cout << "解压后的代码是：" << endl << code2 << endl;
    return 0;
}