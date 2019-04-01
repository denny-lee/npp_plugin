#include <iostream>
#include <string>

using namespace std;

void parseObject(int, char *, int, int, string &);
void parseArray(int, char *, int, int, string &);
void skipSpace(char *);
void printTab(int, string &);
void printError(int, string &);
int getOffset(char *, char);

int total_pos = 0;

int main() {
	//string str("Software\"Microsoft\"Windows NT\"CurrentVersion\"Time Zones\"Eastern Standard Time");
	char * praw = "{\"response\":{\"success\":\"true\",\"info\":\"aaaaa\"},\"extra\":[{\"name\":\"json\",\"type\":\"text\"},{\"name\":\"xml\",\"type\":\"html\"}]}";
	char * p_start = praw;
	//int i = 0;
	int level = 0;
	string result("");
	int i_start = 0;
	int i_end = strlen(praw) - 1;
	parseObject(level, praw, i_start, i_end, result);
	cout << result << endl;
	return 1;
}

void parseObject(int level, char * praw, int start, int end, string &result)
{
	level++;
	result.insert(result.length(), 1, '{');
	result.append("\r\n");
	bool forward = false;
	bool valueFlag = false;
	for (int i = start + 1; i < end; i++) {
		char c = *(praw + i);
		if ('\0' == c) {
			printError(total_pos + i, result);
			return;
		}
		if (forward) {
			if (c == '"') {
				result.insert(result.length(), 1, c);
				forward = false;
			}
			else {
				result.insert(result.length(), 1, c);
			}
		}
		else {
			if (c == '"') {
				if (!valueFlag) {
					printTab(level, result);
				}
				result.insert(result.length(), 1, c);
				forward = true;
			}
			else if (c == ':') {
				if (valueFlag) {
					printError(total_pos, result);
				}
				result.append(": ");
				valueFlag = true;
			}
			else if (',' == c) {
				valueFlag = false;
				result.insert(result.length(), 1, c);
				result.append("\r\n");
			}
			else if ('{' == c) {
				int tmp = i;
				i += getOffset((char *)(praw + i + 1), '}') - 1;
				parseObject(level, praw, tmp, i, result);
			}
			else if ('[' == c) {
				int tmp = i;
				i += getOffset((char *)(praw + i + 1), ']') - 1;
				parseArray(level, praw, tmp, i, result);
			}
			else {
				result.insert(result.length(), 1, c);
			}
		}
		
		
	}
	level--;
	result.append("\r\n");
	printTab(level, result);
	result.insert(result.length(), 1, '}');

	
}

int getOffset(char * praw, char target)
{
	int offset = 1;
	char c = *praw++;
	int level = 0;
	while ('\0' != c) {
		offset++;
		if (target == c && level == 0) {
			break;
		} 
		if (target == '}') {
			if ('{' == c) {
				level++;
			}
			else if ('}' == c) {
				level--;
			}

		}
		else if (target == ']') {
			if ('[' == c) {
				level++;
			}
			else if (']' == c) {
				level--;
			}
		}
		else {
			return 0;
		}
		c = *praw++;
	}
	return offset;
}

void parseArray(int level, char * praw, int start, int end, string & result)
{
	level++;
	result.insert(result.length(), 1, '[');
	result.append("\r\n");
	bool forward = false;
	bool valueFlag = false;
	for (int i = start + 1; i < end; i++) {
		char c = *(praw + i);
		if ('\0' == c) {
			printError(total_pos + i, result);
			return;
		}
		if ('{' == c) {
			printTab(level, result);
			int tmp = i;
			i += getOffset((char *)(praw + i + 1), '}') - 1;
			parseObject(level, praw, tmp, i, result);
		}
		else if (',' == c) {
			result.insert(result.length(), 1, c);
			result.append("\r\n");
		}
		else if ('[' == c) {
			int tmp = i;
			i += getOffset((char *)(praw + i + 1), ']') - 1;
			parseArray(level, praw, tmp, i, result);
		}
	}
	level--;
	result.append("\r\n");
	printTab(level, result);
	result.insert(result.length(), 1, ']');
}

void skipSpace(char * praw)
{
	char c = *praw;
	while (' ' == c) {
		c = *(++praw);
	}
}

void printTab(int level, string & result)
{
	result.insert(result.length(), level, '\t');
}

void printError(int pos, string & result)
{
	char str_pos[6];
	itoa(pos, str_pos, 10);
	result.append("Error at:").append(str_pos);
}