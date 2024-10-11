#include<string.h>
#include<stdio.h>
#define   MAX 32       //关键字数组容量
#define   MAXBUF 255    //输入缓冲区大小

/*
要分析的token类型
关键字keywords
标识符identifiers
常量constants(数字常量。字符常量)
操作符operators
分隔符separators
预处理指令#
*/
char ch;//读入当前的输入字符
int  Line; //当前行数
int  key_count = 0;
int  op_count = 0;
int  pre_count = 0;
int  error_count = 0;
struct keyword{
char lex[MAXBUF];//存放关键字的数组
int token;//关键字序号
};

struct keyword keywords[MAX];
char key_str[MAX][10]={"int","float","double","char","void",
                    "short","long","signed","unsigned",
                    "struct","union", "enum","typedef","sizeof",
                    "auto","static","register","extern","const","volatile",
                    "return","continue","break","goto",
                    "if","else","switch","case","default",
                    "for","while","do"
};
char escape_chars[] = {
        'a',  // 响铃符
        'b',  // 退格符
        'f',  // 换页符
        'n',  // 换行符
        'r',  // 回车符
        't',  // 水平制表符
        'v',  // 垂直制表符
        '\\',  // 反斜杠
        '\'',  // 单引号
        '"',  // 双引号
        '?',  // 问号
        '0'   // 空字符（字符串的结束符）
    };
const int  Identifier=33;
const int  UnsignedInt=34;
const int  Float=35;
const int  ConstantChar=36;
// 转义字符也属于字符常量
const int  ConstantString=37;
enum Ops {
    // 算术运算符
    OP_PLUS = 38,            // +
    OP_MINUS,                // -
    OP_MULTIPLY,             // *
    OP_DIVIDE,               // /
    OP_MODULO,               // %

    // 关系运算符
    OP_EQUAL,                // ==
    OP_NOT_EQUAL,            // "!="
    OP_GREATER,              // >
    OP_LESS,                 // <
    OP_GREATER_EQUAL,        // >=
    OP_LESS_EQUAL,           // <=

    // 逻辑运算符
    OP_LOGICAL_AND,          // &&
    OP_LOGICAL_OR,           // ||
    OP_LOGICAL_NOT,          // "!"

    // 赋值运算符
    OP_ASSIGN,               // =
    OP_PLUS_ASSIGN,          // +=
    OP_MINUS_ASSIGN,         // -=
    OP_MULTIPLY_ASSIGN,      // *=
    OP_DIVIDE_ASSIGN,        // /=
    OP_MODULO_ASSIGN,        // %=
    OP_BITWISE_AND_ASSIGN,   // &=
    OP_BITWISE_OR_ASSIGN,    // |=
    OP_BITWISE_XOR_ASSIGN,   // ^=
    OP_LEFT_SHIFT_ASSIGN,    // <<=
    OP_RIGHT_SHIFT_ASSIGN,   // >>=

    // 自增和自减运算符
    OP_INCREMENT,            // ++
    OP_DECREMENT,            // --

    // 位运算符
    OP_BITWISE_AND,          // &
    OP_BITWISE_OR,           // |
    OP_BITWISE_XOR,          // ^
    OP_BITWISE_NOT,          // ~
    OP_LEFT_SHIFT,           // <<
    OP_RIGHT_SHIFT,          // >>

    // 条件（三元）运算符
    OP_CONDITIONAL,          // "? :"

    // 指针运算符
    OP_DEREFERENCE,          // *
    OP_ADDRESS_OF,           // &

    // 成员访问运算符
    OP_MEMBER_ACCESS,        // .
    OP_POINTER_MEMBER_ACCESS,// ->

    // 分隔符
    OP_COMMA,                // ,
    OP_SEMICOLON,            // ;
    OP_PARENTHESES_LEFT,     // (
    OP_PARENTHESES_RIGHT,    // )
    OP_BRACES_LEFT,          // {
    OP_BRACES_RIGHT,         // }
    OP_BRACKETS_LEFT,        // [
    OP_BRACKETS_RIGHT,       // ]
    OP_COLON,                // :

    // 预处理指令标识符
    OP_PREPROCESSOR_ID,       //#

    OP_DOUBLE_QUOTE,          // "
    OP_SINGLE_QUOTE,          // '
};

enum Errors
{
    INVALID_CHARACTER = 1,
    UNTERMINATED_COMMENT,
    INVALID_IDENTIFIER,
    INVALID_NUMERIC_CONSTANT,
    INVALID_STRING_LITERAL,
    INVALID_CHARACTER_CONSTANT,
    UNKNOWN_OPERATOR,
    INVALID_ESCAPE_SEQUENCE,
};
void init()
{
    int j;
	for(j=0; j<MAX; j++)
	{
	strcpy(keywords[j].lex,key_str[j]);
	keywords[j].token=j+1;
	}
}

int IsKeyword(char* res){
    int i;
    for(i=0;i<MAX;i++){
        if((strcmp(keywords[i].lex, res))==0) break;
    }
    if(i<MAX)
    {
        return   keywords[i].token;
    }
    else return 0;
}

int IsLetter(char c)
{
    if(((c<='z')&&(c>='a'))||((c<='Z')&&(c>='A'))) return 1;
    else return 0;
}

int IsDigit(char c){
    if(c>='0'&&c<='9') return 1;
    else return 0;
}

int IsEscape(char c){
    for(int i = 0; i < 12;i ++)
    {
        if(c == escape_chars[i]) return 1;
    }
    return 0;
}

void Analyze(FILE *fpin,FILE *fpout){
    char array[MAXBUF];//当前要分析的token
    int j=0;
    while((ch=fgetc(fpin))!=EOF){
    // printf("ch = %c\n",ch);
    if(ch==' '||ch=='\t'){
        continue;
    }
    else if(ch=='\n'){
        Line++;
    }
    else if(IsLetter(ch)){
        //读入标识符,从字符角度来看，关键字也是一种特殊的标识符
        while(IsLetter(ch) || IsDigit(ch) || ch=='_'){
		if((ch<='Z')&&(ch>='A')){
		ch=ch+32;
        }
            array[j]=ch;
            j++;
            ch=fgetc(fpin);
        }
        array[j]='\0';

        ungetc(ch,fpin);
        j=0;
        if (IsKeyword(array)){
            key_count++;
            fprintf(fpout,"Keyword:%s \t\t token_id: %d\n",array,IsKeyword(array));
        }else
			fprintf(fpout,"Identifier:%s \t\t token_id: %d\n",array,Identifier);//标识符
    }
    else if(IsDigit(ch)){
	int s=0;
    while(IsDigit(ch) || IsLetter(ch)){
	if(IsLetter(ch)){
            array[j]=ch;
            j++;
            ch=fgetc(fpin);
			s=1;
		}
	else if(IsDigit(ch)){
			array[j]=ch;
            j++;
            ch=fgetc(fpin);

            if(ch=='.')
			{
                s = 2;
				array[j]=ch;
				j++;
                // printf("ch = %c\n",ch);
                ch=fgetc(fpin);
			}
            if (ch == 'x' || ch == 'X')
            {
                s = 3;
				array[j]=ch;
				j++;
				ch=fgetc(fpin);
                
				while(IsDigit(ch) || ch =='a' || ch == 'A' || ch == 'b' || ch == 'B' || ch == 'c' || ch == 'C' || ch == 'd' || ch == 'D' || ch == 'e' || ch == 'E' || ch == 'f' || ch == 'F')
				{
					array[j]=ch;
					j++;
					ch=fgetc(fpin);
				}
            }
		}
    }
        array[j]='\0';
        ungetc(ch,fpin);
        j=0;
	if(s==0)
		fprintf(fpout,"Unsigned integer: %s\t token_id: %d\n",array,UnsignedInt);//无符号整数
    else if(s==3)
        fprintf(fpout,"Unsigned xdigit: %s\t token_id: %d\n",array,UnsignedInt);//无符号十六进制整数
    else if(s==2)
        fprintf(fpout,"Float: %s\t token_id: %d\n",array,Float);//浮点数
	else if(s==1){
		fprintf(fpout,"Error:%s\t\t Line: %d\t Error id: %d\n",array,Line,INVALID_IDENTIFIER);
        error_count++;
    }
    }


    else switch(ch){
        //需要超前分析的
        case'+' :{
            op_count++;
            ch=fgetc(fpin);
            if(ch=='+'){
            fprintf(fpout,"Operator: %s\t token_id: %d\n","++",OP_INCREMENT);
            break;
			}
			else if(ch=='='){
			fprintf(fpout,"Operator: %s\t token_id: %d\n","+=",OP_PLUS_ASSIGN);
            break;
			}
            else
			fprintf(fpout,"Operator: %s\t token_id: %d\n","+",OP_PLUS);
			break;
        }
        case'-' :{
            op_count++;
            ch=fgetc(fpin);
            if(ch=='-'){
            fprintf(fpout,"Operator: %s\t token_id: %d\n","--",OP_DECREMENT);
            break;
			}
			else if(ch=='='){
			fprintf(fpout,"Operator: %s\t token_id: %d\n","-=",OP_MINUS_ASSIGN);
            break;
			}
            else
			fprintf(fpout,"Operator: %s\t token_id: %d\n","-",OP_MINUS);
			break;
        }
        case'*' :{
            op_count++;
            ch=fgetc(fpin);
            if(ch=='='){
            fprintf(fpout,"Operator: %s\t token_id: %d\n","*=",OP_MULTIPLY_ASSIGN);
            break;
			}
            else
			fprintf(fpout,"Operator: %s\t token_id: %d\n","*",OP_MINUS);
			break;
        }
        case'/' :{
            op_count++;
			ch=fgetc(fpin);
            if(ch=='='){
            fprintf(fpout,"Operator: %s\t token_id: %d\n","/=",OP_DIVIDE_ASSIGN);
            break;
			}
			else if(ch=='/'){
                char commits1[MAXBUF];
                int i = 0;
                while(ch!='\n' && ch!=EOF){
					ch=fgetc(fpin);
                    commits1[i++] = ch; 
					}
                commits1[i]='\0';
                fprintf(fpout,"Comment: %s\n",commits1);
			}
			else if(ch=='*'){
                char commits2[MAXBUF];
                int i = 0;
                while(1){
				ch=fgetc(fpin);
                commits2[i++] = ch;
                if(ch==EOF){
                fprintf(fpout,"Error: Incomplete comment!\t Line: %d\t\t Error id: %d\n",Line,UNTERMINATED_COMMENT);
                error_count++;
                }
				if(ch == '*'){
                    ch=fgetc(fpin);
                    if(ch=='/')
                    {
                        fprintf(fpout,"Comment: %s\n",commits2);
                        break;
                    }
                }
                }
			}
            else{
                fprintf(fpout,"Operator: %s\t token_id: %d\n","/",OP_DIVIDE);
            }
			break;
        }
        case'>' :{
            op_count++;
			ch=fgetc(fpin);
            if(ch=='=')
                fprintf(fpout,"Operator: %s\t token_id: %d\n",">=",OP_GREATER_EQUAL);
            else if(ch == '>')
                fprintf(fpout,"Operator: %s\t token_id: %d\n",">>",OP_RIGHT_SHIFT);
            else {
                    fprintf(fpout,"Operator: %s\t token_id: %d\n",">",OP_GREATER);
                    }
            break;
        }

        case'<' :{
            op_count++;
			ch=fgetc(fpin);
            if(ch=='=')
                fprintf(fpout,"Operator: %s\t token_id: %d\n","<=",OP_LESS_EQUAL);
            else if(ch=='<')
                fprintf(fpout,"Operator: %s\t token_id: %d\n","<<",OP_LEFT_SHIFT);
            else{
                fprintf(fpout,"Operator: %s\t token_id: %d\n","<",OP_LESS);
                }
            break;
        }
        case'=' :{
            op_count++;
            ch=fgetc(fpin);
            if(ch=='=')
                fprintf(fpout,"Operator: %s\t token_id: %d\n","==",OP_EQUAL);
            else
                fprintf(fpout,"Operator: %s\t token_id: %d\n","=",OP_ASSIGN);
            break;
        }
        case'&' :{
            op_count++;
            ch = fgetc(fpin);
            if(ch == '&')
            fprintf(fpout,"Operator: %s\t token_id: %d\n","&",OP_LOGICAL_AND);
            else
            fprintf(fpout,"Operator: %s\t token_id: %d\n","&",OP_ADDRESS_OF);
            break;
        }
        case'|' :{
            op_count++;
            ch = fgetc(fpin);
            if(ch == '|')
            fprintf(fpout,"Operator: %s\t token_id: %d\n","&",OP_LOGICAL_OR);
            break;
        }
        case'!' :{
        op_count++;
        ch=fgetc(fpin);
        if(ch=='=')
            fprintf(fpout,"Operator: %s\t token_id: %d\n","!=",OP_NOT_EQUAL);
        else
            fprintf(fpout,"Operator: %s\t token_id: %d\n","!",OP_LOGICAL_NOT);
        break;
        }
        case'#' :{
            op_count++;
            char buffer[MAXBUF];

            buffer[0] = '#';
            int i = 1;
            
            Line++;
            while ((ch = fgetc(fpin)) != '\n' && ch != EOF) {
                buffer[i++] = ch;
            }
            buffer[i] = '\0';  // 结束字符串
            fprintf(fpout,"Preprocessor Line: %s\t token_id: %d\n",buffer,OP_PREPROCESSOR_ID);
            pre_count++;
            break;
        }
        
        case'"' :{
            fprintf(fpout,"Operator: %s\t token_id: %d\n","\"",OP_DOUBLE_QUOTE);
            op_count++;
            char buffer[MAXBUF];
            memset(buffer, '\0', sizeof(buffer));
            buffer[0] = '"';
            int i = 1;
            while ((ch = fgetc(fpin)) != '"' && ch != EOF && ch != '\n' && ch != ';') {
                if(ch == '\\')
                {
                    buffer[i++] = ch;
                    ch = fgetc(fpin);
                    if(IsEscape(ch) == 0)
                    {
                    fprintf(fpout,"Error: Invalid Escape Sequence -- \\%c\t Line: %d\t\t Error id: %d\n",ch,Line,INVALID_ESCAPE_SEQUENCE);
                    }
                }
                buffer[i++] = ch;
            }

            if(ch != '"')
            {
            fprintf(fpout,"Error: Invalid String Literal!\t Line: %d\t\t Error id: %d\n",Line,INVALID_STRING_LITERAL);
            error_count++;
            ungetc(ch,fpin);
            }
            else
            {
                buffer[i] = ch;
                fprintf(fpout,"Constant String: %s\t token_id: %d\n",buffer,ConstantString);
                fprintf(fpout,"Operator: %s\t token_id: %d\n","\"",OP_DOUBLE_QUOTE);
                op_count++;
            }
            break;
        }
        case'\'' :{
            fprintf(fpout,"Operator: %s\t token_id: %d\n","\'",OP_SINGLE_QUOTE);
            op_count++;
            ch = fgetc(fpin);
            if(ch == EOF){
            fprintf(fpout,"Error: Invalid String Literal!\t Line: %d\t\t Error id: %d\n",Line,INVALID_STRING_LITERAL);
            error_count++;
            }
            else if(ch == '\\')
            {
                ch = fgetc(fpin);
                if(ch == EOF){
                fprintf(fpout,"Error: Invalid String Literal!\t Line: %d\t\t Error id: %d\n",Line,INVALID_STRING_LITERAL);
                error_count++;
                }
                else if(IsEscape(ch) == 0)
                {
                    fprintf(fpout,"Error: Invalid Escape Sequence -- \\%c\t Line: %d\t\t Error id: %d\n",ch,Line,INVALID_ESCAPE_SEQUENCE);
                }
                else
                {
                    fprintf(fpout,"Constant Char: \\%c\t token_id: %d\n",ch,ConstantChar);
                }
            }
            else{
                fprintf(fpout,"Constant Char: %c\t token_id: %d\n",ch,ConstantChar);
            }
            ch = fgetc(fpin);
            if(ch != '\''){
            fprintf(fpout,"Error: Invalid String Literal!\t Line: %d\t\t Error id: %d\n",Line,INVALID_STRING_LITERAL);
            error_count++;
            ungetc(ch,fpin);
            }
            else
            {
                fprintf(fpout,"Operator: %s\t token_id: %d\n","\'",OP_SINGLE_QUOTE);
                op_count++;
            }
            break;
        }
        case'(' :op_count++;fprintf(fpout,"Operator: %s\t token_id: %d\n","(",OP_PARENTHESES_LEFT);break;
        case')' :op_count++;fprintf(fpout,"Operator: %s\t token_id: %d\n",")",OP_PARENTHESES_RIGHT);break;
        case'[' :op_count++;fprintf(fpout,"Operator: %s\t token_id: %d\n","[",OP_BRACKETS_LEFT);break;
        case']' :op_count++;fprintf(fpout,"Operator: %s\t token_id: %d\n","]",OP_BRACKETS_RIGHT);break;
        case';' :op_count++;fprintf(fpout,"Operator: %s\t token_id: %d\n",";",OP_SEMICOLON);break;
        case'.' :op_count++;fprintf(fpout,"Operator: %s\t token_id: %d\n",".",OP_MEMBER_ACCESS);break;
        case',' :op_count++;fprintf(fpout,"Operator: %s\t token_id: %d\n",",",OP_COMMA);break;
		case':' :op_count++;fprintf(fpout,"Operator: %s\t token_id: %d\n",":",OP_COLON);break;
		case'{' :op_count++;fprintf(fpout,"Operator: %s\t token_id: %d\n","{",OP_BRACES_LEFT);break;
		case'}' :op_count++;fprintf(fpout,"Operator: %s\t token_id: %d\n","}",OP_BRACES_RIGHT);break;
		case'%' :op_count++;fprintf(fpout,"Operator: %s\t token_id: %d\n",'%',OP_MODULO);break;
        default :
            fprintf(fpout,"Error: %c\t\t Line: %d\t Error id: %d\n",ch,Line,INVALID_CHARACTER);
            error_count++;
        }
    }
}
int main(){
    char in_fn[25],out_fn[25];
    FILE * fpin,* fpout;
    printf("..............C语言词法分析程序......................\n");
    printf("\n");
    printf(".....全部输入输出程序都在当前文件夹下.....\n");
    printf(".....测试程序: test.txt \n");
    printf(".....输入要进行词法分析的程序:\n");
    scanf("%s",in_fn);
    printf(".....输入要保存的目标文件名:\n");
    scanf("%s",out_fn);
    fpin=fopen(in_fn,"r");
    fpout=fopen(out_fn,"w");
    fprintf(fpout,"Results of lexical analysis of C language programs\n");
    fprintf(fpout,"\n");
    init();
    Analyze(fpin,fpout);
    fprintf(fpout,"\n");
    fprintf(fpout,"\n");
    fprintf(fpout,"Line: %d\n",Line);
    fprintf(fpout,"Preprocessor Line: %d\n",pre_count);
    fprintf(fpout,"key_word: %d\n",key_count);
    fprintf(fpout,"operator: %d\n",op_count);
    fprintf(fpout,"error: %d\n",error_count);
    fclose(fpin);
    fclose(fpout);
    printf(".....程序已分析完成分析并保存至目标文件\n");
    printf("........END......\n");
    return 0;
}
