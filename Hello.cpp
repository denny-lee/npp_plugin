#include <iostream>
#include <string>

using namespace std;

int main() {
	//string str("Software\"Microsoft\"Windows NT\"CurrentVersion\"Time Zones\"Eastern Standard Time");
	char * raw = "{\"response\":{\"success\":\"true\",\"info\":\"aaaaa\"},\"extra\":[{\"name\":\"json\",\"type\":\"text\"},{\"name\":\"xml\",\"type\":\"html\"}]}";
	//int i = 0;
	int level = 0;
	bool newLine = false;
	bool inArr = false;
	string result("");
	while(true) 
	{
		char c = *raw++;
		if ('\0' == c) break;
		if (newLine) {
			newLine = false;
			for (int j = 0; j < level; j++) {
				result.insert(result.length(), 1, '\t');
			}
		}
		if ('{' == c) {
			result.insert(result.length(), 1, c);
			result.append("\r\n");
			newLine = true;
			level++;
			
		} else if ('}' == c) {
			result.append("\r\n");
			level--;
			for (int j = 0; j < level; j++) {
				result.insert(result.length(), 1, '\t');
			}
			result.insert(result.length(), 1, c);
		}
		else if (',' == c) {
			result.insert(result.length(), 1, c);
			if (!inArr) {
				result.append("\r\n");
			}
			newLine = true;
		}
		else if ('[' == c) {
			result.insert(result.length(), 1, c);
			inArr = true;
		}
		else if (']' == c) {
			result.insert(result.length(), 1, c);
			inArr = false;
		}
		else {
			
			result.insert(result.length(), 1, c);
		}
	}
	cout << result << endl;
	return 1;
}