#include<stdio.h>
#include<string.h>
#include "sqlite3.h"
#pragma comment(lib, "sqlite3.lib")

void outputTable();
void insertData();
void deleteData();
void outputOneData();
void outputID();

sqlite3* db;   //データベースのハンドル

int main(void) {
	char* filename = "test.db";
	char* err = "error";
	int rc; //result codes
	int tc;	//table create

	int modeChoice = 0;


	//データベースを開く
	rc = sqlite3_open(filename, &db);
	if (rc != SQLITE_OK) {
		sqlite3_close(db);
		printf("error_1");
		return 1;
	}

	//テーブルの作成
		/*tc = sqlite3_exec(db, "create table test(quizNumber,Q,A);",NULL,NULL,&err);
		if (tc != SQLITE_OK) {
			sqlite3_close(db);
			printf("error_createTable");
			return 1;
		}*/

	printf("暗記アプリ\n");
	printf("---------------------\n\n");

	while (1) {
		sqlite3_stmt* stmt;
		int rc = 0; //result code
		int i = 0;//問題番号の表示用
		rc = sqlite3_prepare_v2(db, "select quizNumber,Q,A from test", -1, &stmt, 0);
		if (rc != SQLITE_OK) {
		}
		rc = sqlite3_step(stmt);
		while (rc == SQLITE_ROW)
		{
			i++;
			rc = sqlite3_step(stmt);
		}

		printf("  現在%d問保存されています\n\n", i);
		printf("  1: 解答する\n");
		printf("  2: 問題IDを指定して解答する\n");
		printf("  3: 問題を一覧表示する\n");
		printf("  4: 問題の追加\n");
		printf("  5: 問題の削除\n");
		printf("  0: 終了\n\n");

		printf("  機能の番号を選択してください...");
		scanf_s("%d", &modeChoice);

		if (modeChoice == 0) {
			break;
		}

		if (modeChoice == 1) {
			outputOneData();
		}

		if (modeChoice == 2) {
			outputID();
		}

		if (modeChoice == 3) {
			outputTable();
		}

		if (modeChoice == 4) {
			insertData();
		}
		if (modeChoice == 5) {
			deleteData();
		}

	}

	//データベースを閉じる
	sqlite3_close(db);
	return 0;
}



void outputTable() {
	sqlite3_stmt* stmt;
	int rc = 0; //result code
	int i = 0;//問題番号の表示用

	//プリペアドステートメントを生成
	rc = sqlite3_prepare_v2(db, "select quizNumber,Q,A from test", -1, &stmt, 0);
	if (rc != SQLITE_OK) {
		//return 1;
	}

	//行を読み取る。
	rc = sqlite3_step(stmt);

	printf("\n");
	while (rc == SQLITE_ROW)
	{
		i++;
		printf("--------------------------------------\n");
		printf("  %d\n", i);
		printf("  問題ID : %s\n", sqlite3_column_text(stmt, 0));
		printf("  問題文 : %s\n", sqlite3_column_text(stmt, 1));
		printf("  答え : %s", sqlite3_column_text(stmt, 2));
		printf("\n");
		rc = sqlite3_step(stmt);
	}
	printf("--------------------------------------\n");
	printf("\n");

	//プレペアドステートメントの解放
	rc = sqlite3_finalize(stmt);
	if (rc != SQLITE_OK) {
		printf("ERROR(%d) %s\n", rc, sqlite3_errmsg(db));
	}
}



void insertData() {

	int is; //insert
	char* err = "error_insertData";
	char insMsg_0[600] = "";
	char insMsg_1[256] = { "insert into test(quizNumber,Q,A)values('" };
	char insMsg_2[100] = { "" };	//クイズ番号
	char insMsg_3[4] = "','";
	char insMsg_4[100] = "";	//問題文
	char insMsg_5[4] = "','";
	char insMsg_6[100] = "";	//答え
	char insMsg_7[4] = "');";

	printf("  問題IDの入力\n");
	scanf_s("%s", &insMsg_2, 100);

	printf("  問題文の入力\n");
	scanf_s("%s", &insMsg_4, 100);

	printf("  答えの入力\n");
	scanf_s("%s", &insMsg_6, 100);

	if (insMsg_2 != "" && insMsg_4 != "" && insMsg_6 != "") {
		snprintf(insMsg_0, 200, "%s%s%s%s%s%s%s",
			insMsg_1, insMsg_2, insMsg_3, insMsg_4, insMsg_5, insMsg_6, insMsg_7);

		is = sqlite3_exec(db, insMsg_0, NULL, NULL, &err);
		if (is != SQLITE_OK) {
			sqlite3_close(db);
			printf("error_3_ins\n");
		}
	}
	else {
		printf("追加時エラー\n");
	}


}

void deleteData() {

	int deleteMsg;
	char* err = "error_deleteData()";

	char i = 0;
	char delMsg_1[100] = { "delete from test where quizNumber ='" };
	char delMsg_2[10] = { "" };
	char delMsg_3[10] = { "';" };

	printf("  削除したい問題IDの入力...");
	scanf_s("%s", &delMsg_2, 10);
	printf("\n");
	//i = delMsg_2;

	strcat_s(delMsg_1, sizeof(delMsg_1), (delMsg_2));
	strcat_s(delMsg_1, sizeof(delMsg_1), (delMsg_3));

	deleteMsg = sqlite3_exec(db, delMsg_1, NULL, NULL, &err);

	printf("  問題ID %s を削除しました\n\n", delMsg_2);

	if (deleteMsg != SQLITE_OK) {
		sqlite3_close(db);
		printf("%s", delMsg_1);
		printf("\nerror_3_dlt");
	}
}

void outputOneData() {
	sqlite3_stmt* stmt;
	int rc = 0; //result code
	int i = 0;//問題番号の表示用

	char outputMsg_1[100] = { "select * from test " };

	//データベースから取得した答えを保存する変数
	int			newAns1 = 0;
	const char* newAns2 = "";
	char* newAns3 = "";
	char* newAns99 = "";

	//入力した答えを保存する変数
	char Ans[164] = { "a" };
	unsigned int size = 164;

	//正答率を出すための変数
	int nice_Q = 0;
	int bad_Q = 0;
	double cnt_Q = 0;


	//プリペアドステートメントを生成
	rc = sqlite3_prepare_v2(db, outputMsg_1, -1, &stmt, 0);
	if (rc != SQLITE_OK) {
		//return 1;
	}

	//行を読み取る。
	rc = sqlite3_step(stmt);
	printf("\n");

	newAns1 = sqlite3_column_int(stmt, 0);		//問題ID
	newAns2 = sqlite3_column_text(stmt, 1);		//問題文
	newAns3 = sqlite3_column_text(stmt, 2);		//答え
	while (rc == SQLITE_ROW)
	{
		i++;
		cnt_Q++;
		printf("--------------------------------------\n");
		//printf("  問題ID : %d\n", i);
		//printf("  指定用問題ID : %s\n", sqlite3_column_text(stmt, 0));
		printf("  問題 : %s\n", sqlite3_column_text(stmt, 1));

		newAns99 = newAns3;
		printf("  答え : ");
		scanf_s("%s", &Ans, size);

		if (strcmp(newAns99, Ans) == 0) {
			printf("  正解\n");
			nice_Q++;
		}
		else {
			printf("  不正解\n");
			printf("  正解は...%s", newAns99);
			bad_Q++;
		}

		printf("\n\n");
		rc = sqlite3_step(stmt);
	}

	if (bad_Q > 0) {
		printf("  正解数..%d 問\n", nice_Q);
		if (nice_Q == 0) {
			printf("  正解率..0 %%\n");
			printf("  不正解数..%d 問\n", bad_Q);
			printf("  不正解率..100 %%\n");
		}
		else {
			printf("  正解率..%.1lf %%\n", nice_Q / cnt_Q * 100);
			printf("  不正解数..%d 問\n", bad_Q);
			printf("  不正解率.. %.1lf %%\n", bad_Q / cnt_Q * 100);
		}
	}
	else {
		printf("  正解数..%d 問\n", nice_Q);
		printf("  正解率..100 %%\n");
		printf("  不正解数..%d 問\n", bad_Q);
		printf("  不正解率..0 %%\n");
	}

	printf("\n\n");

	//---------------------------------------------------------------------------------------------------------------


	//プレペアドステートメントの解放
	rc = sqlite3_finalize(stmt);
	if (rc != SQLITE_OK) {
		//printf("ERROR(%d) %s\n", rc, sqlite3_errmsg(db));
		printf("ERROR\n");

	}
}

void outputID() {
	sqlite3_stmt* stmt;
	int rc = 0; //result code
	int i = 0;//問題番号の表示用

	char outputMsg_1[100] = "select * from test where quizNumber = '";
	char outputMsg_2[30] = "";
	char outputMsg_3[4] = "';";

	//データベースから取得した答えを保存する変数
	int			newAns1 = 0;
	const char* newAns2 = "";
	char* newAns3 = "";
	char* newAns99 = "";

	//入力した答えを保存する変数
	char Ans[164] = { "a" };
	unsigned int size = 164;

	//正答率を出すための変数
	int nice_Q = 0;
	int bad_Q = 0;
	double cnt_Q = 0;

	//問題IDの指定
	printf("  問題のIDを入力してください...");
	scanf_s("%s", &outputMsg_2, 20);

	strcat_s(outputMsg_2, sizeof(outputMsg_2), outputMsg_3);
	//printf("outputMsg_2 = %s\n", outputMsg_2);
	strcat_s(outputMsg_1, sizeof(outputMsg_1), outputMsg_2);
	//printf("outputMsg_1 = %s\n", outputMsg_1);

	//プリペアドステートメントを生成
	rc = sqlite3_prepare_v2(db, outputMsg_1, -1, &stmt, 0);
	if (rc != SQLITE_OK) {
		//return 1;
	}

	//行を読み取る。
	rc = sqlite3_step(stmt);
	printf("\n");

	newAns1 = sqlite3_column_int(stmt, 0);		//問題ID
	newAns2 = sqlite3_column_text(stmt, 1);		//問題文
	newAns3 = sqlite3_column_text(stmt, 2);		//答え
	while (rc == SQLITE_ROW)
	{
		i++;
		cnt_Q++;
		printf("--------------------------------------\n");
		//printf("  問題ID : %d\n", i);
		//printf("  指定用問題ID : %s\n", sqlite3_column_text(stmt, 0));
		printf("  問題 : %s\n", sqlite3_column_text(stmt, 1));
		newAns99 = newAns3;
		printf("  答え : ");
		scanf_s("%s", &Ans, size);

		if (strcmp(newAns99, Ans) == 0) {
			printf("  正解\n");
			nice_Q++;
		}
		else {
			printf("  不正解\n");
			printf("  正解は...%s", newAns99);
			bad_Q++;
		}

		printf("\n\n");
		rc = sqlite3_step(stmt);
	}


	if (bad_Q > 0) {
		printf("  正解数..%d 問\n", nice_Q);
		if (nice_Q == 0) {
			printf("  正解率..0 %%\n");
			printf("  不正解数..%d 問\n", bad_Q);
			printf("  不正解率..100 %%\n");
		}
		else {
			printf("  正解率..%.1lf %%\n", nice_Q / cnt_Q * 100);
			printf("  不正解数..%d 問\n", bad_Q);
			printf("  不正解率.. %.1lf %%\n", bad_Q / cnt_Q * 100);
		}
	}
	else {
		printf("  正解数..%d 問\n", nice_Q);
		printf("  正解率..100 %%\n");
		printf("  不正解数..%d 問\n", bad_Q);
		printf("  不正解率..0 %%\n");
	}

	printf("\n\n");

	//---------------------------------------------------------------------------------------------------------------


	//プレペアドステートメントの解放
	rc = sqlite3_finalize(stmt);
	if (rc != SQLITE_OK) {
		//printf("ERROR(%d) %s\n", rc, sqlite3_errmsg(db));
		printf("ERROR\n");

	}
}