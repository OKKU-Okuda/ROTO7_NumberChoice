// roto7.cpp 
//
#include <string.h>
#include <conio.h>

#include <random>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#define PICKUPPATH   ("PickUpNumber.txt")

//@brief 回す際の実行データ
struct ExsecData {
   int MinNumber = 1;   //!< 最小選出値
   int MaxNumber = 37;  //!< 最大選出値
   int CountRot = 7;    //!< 数値を回す回数
   std::vector<int> PickUp;   //!< ピックアップ対象
   float PickUpRate = 1.3f;      //!< ピックアップ数値の選出確率
};

void LoadFile();
void DrawFile();
void CheckExsecData();
void Choice();

ExsecData exsec;
std::vector<int> choice;

int main()
{
   LoadFile();

   CheckExsecData();

   Choice();

   for (unsigned int idx = 0u; idx < choice.size(); ++idx)
   {
      std::cout << (idx + 1) << ":" << choice[idx] << std::endl;
   }
   // キー入力待ち
   rewind(stdin);
   return _getch();
}

void Choice()
{
   std::vector<int> NumNormal;   // 通常数字配列
   std::vector<int> NumPickUp;   // ピックアップ数字配列

   // 数字の分別
   for (int num = exsec.MinNumber; num <= exsec.MaxNumber; ++num)
   {// 数字の分別
      bool isPickNum = false;
      for (auto& pickarray : exsec.PickUp)
      {
         if (num != pickarray)
         {// 数値がピック数字ではない
            continue;
         }
         // ピック数値
         isPickNum = true;
         break;
      }

      if (isPickNum)
      {// ピック配列に代入
         NumPickUp.push_back(num);
         continue;
      }

      // 通常配列に代入
      NumNormal.push_back(num);
   }

   // 一様分布
   std::uniform_real_distribution<> dist(0.f, static_cast<float>(NumNormal.size()) + (exsec.PickUpRate * static_cast<float>(NumPickUp.size())));

   std::random_device rng;    //!< 完全な乱数生成器

   // 回数分回す
   while (choice.size() < static_cast<unsigned int>(exsec.CountRot))
   {
     double random = dist(rng);

     int numSelect = 0;

     if (static_cast<unsigned int>(random) < NumNormal.size())
     {// 通常配列数字からピックアップ
        numSelect = NumNormal[static_cast<unsigned int>(random)];
     }
     else
     {// ピックアップ配列からピックアップ
        double pickrandom = random - static_cast<double>(NumNormal.size());

        unsigned int idx = static_cast<unsigned int>(pickrandom / static_cast<double>(exsec.PickUpRate));

        numSelect = NumPickUp[idx];
     }

     bool isDouble = false;
     for (auto& num : choice)
     {
        if (num != numSelect)
        {
           continue;
        }

        isDouble = true;
        break;
     }

     if (!isDouble)
     {// 重なっていないので追加
        choice.push_back(numSelect);
     }
   }

   // ソート
   std::sort(choice.begin(), choice.end());
}

void CheckExsecData()
{
   // 重圧は０以下だとバグる
   // ほんとは１で調整しようと思ったけどあたりにくい数字を指定できるように調整
   if (exsec.PickUpRate <= 0.f)
   {
      exsec.PickUpRate = 0.000000000000001f;
   }

   // 選出回数は１回以上
   if (exsec.CountRot <= 0)
   {
      exsec.CountRot = 1;
   }

   // ピックアップに同じ値がないかチェック(あれば消す）
   std::vector<int> NewPickUp;   

   for (unsigned int idx = 0u; idx < exsec.PickUp.size(); ++idx)
   {
      bool isBanNumber = false;

      if (exsec.PickUp[idx] > exsec.MaxNumber || exsec.PickUp[idx] < exsec.MinNumber)
      {// 数値が範囲外
         isBanNumber = true;
      }
      else
      {
         for (auto& itr : NewPickUp)
         {
            if (itr == exsec.PickUp[idx])
            {// 重なっている
               isBanNumber = true;
               break;
            }
         }
      }

      if (isBanNumber)
      {
         continue;
      }

      // 重複がない場合は代入
      NewPickUp.push_back(exsec.PickUp[idx]);
   }

   // 配列を重複内versionに変更
   exsec.PickUp = NewPickUp;

}

void LoadFile()
{
   std::ifstream text_numPickUp(PICKUPPATH);     //!< ピックアップ数字を入れているファイル

   if (!text_numPickUp)
   {
      std::cout << "ファイルが開けないため新規ファイル作成しました。" << std::endl;
      DrawFile();

      text_numPickUp.close();
      text_numPickUp.open(PICKUPPATH);
   }

   char text[1024];
   // 解析
   while (true)
   {
      text_numPickUp.getline(text, sizeof(text));

      if (std::string(text) == "最小値")
      {
         text_numPickUp.getline(text, sizeof(text));
         sscanf_s(text, "%d", &exsec.MinNumber);
      }
      else if (std::string(text) == "最大値")
      {
         text_numPickUp.getline(text, sizeof(text));
         sscanf_s(text, "%d", &exsec.MaxNumber);
      }
      else if (std::string(text) == "ロット回数")
      {
         text_numPickUp.getline(text, sizeof(text));
         sscanf_s(text, "%d", &exsec.CountRot);
      }
      else if (std::string(text) == "ピックアップ数値重圧")
      {
         text_numPickUp.getline(text, sizeof(text));
         sscanf_s(text, "%f", &exsec.PickUpRate);
      }
      else if (std::string(text) == "ピックアップ")
      {
         while (1)
         {
            text_numPickUp.getline(text, sizeof(text));

            if (std::string(text) == "" && text_numPickUp.eof())
            {
               break;
            }
            unsigned int num;
            sscanf_s(text, "%d", &num);

            exsec.PickUp.push_back(num);
         }
      }

      if (text_numPickUp.eof())
      {// 終了
         break;
      }
   }
   text_numPickUp.close();
}

void DrawFile()
{
   static constexpr char DefaultText[] = {
      "最小値\n1\n\n最大値\n37\n\nロット回数\n7\n\nピックアップ数値重圧\n1.3\n\nピックアップ\n15\n9\n4\n13\n21\n30\n23\n35\n10\n34\n36" };
   std::ofstream text_numPickUp(PICKUPPATH);     //!< ピックアップ数字を入れているファイル

   text_numPickUp.write(DefaultText, sizeof(DefaultText));
   text_numPickUp.close();
}