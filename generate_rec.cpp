/*************************************************************
*
*generate_rec.cpp: generate rec
*auther     : caowg
*Written on : 05/21/14.
*
*************************************************************/

#include <iostream>
#include <fstream>
#include "rec_packing.H"
#include <time.h>
#include <cstdlib>

using namespace std;

vector< vector<rectangle> > g_vrecset;
vector< rectangle> g_rec ;

int g_width;
int g_height;
int g_opt_time;

int g_rec_number_uplimit;

// 对矩形块集合进行分割
void rec_sub();

//随机 分割矩形块 rec 为两个小举行块rec1 rec2
bool rec_cut(const rectangle rec, rectangle & rec1 ,rectangle & rec2);

// 集合合并
void rec_merge();

// 初始化，即输入数据:矩形框宽，高，最优解时间，
// 以及最后分割后矩形块个数的上限
void init();

// 将集合中的数据放入 vector中
void push_data2vec();

// 将 vector中数据进行随机洗牌
void shullfle_datas();


// 输出数据到文件中
// 第一行：矩形框 宽，高，最优解
// 第二行：矩形块个数
void output_data();

// 矩形块个数
int get_rec_number();

// 检查分割算法是否正确
bool check();

int main(int arg ,char *arv[])
{
    init();
    rec_sub();
    rec_merge();
    push_data2vec();
    shullfle_datas();
    output_data();
    check();
    return 0;
            
    
}


void init()
{
    cout<<"please input the big rec width,height,opt_time & rec up limit:"<<endl;
    cin>>g_width;
    cin>>g_height>>g_opt_time>>g_rec_number_uplimit;
    if (g_rec_number_uplimit > g_width* g_height * g_opt_time)
        g_rec_number_uplimit = g_width* g_height * g_opt_time;
    
    vector<rectangle> v_rec;
    rectangle rec;
    rec.width = g_width;
    rec.height = g_height;
    rec.time = 1;
    v_rec.push_back(rec);
    for (int i = 0; i< g_opt_time; ++i)
        g_vrecset.push_back(v_rec);
    
}

int get_rec_number()
{
    int number = 0;
    for (vector< vector<rectangle> >::iterator it = g_vrecset.begin();
         it != g_vrecset.end(); ++it)
        number += it->size();
    return number;
}

void rec_sub()
{
    int set_cur = 0;
    int rec_cur = 0;
    srand((int)time(0));
    vector<rectangle> ::iterator it ;
    rectangle rec1, rec2;
    while(get_rec_number() < g_rec_number_uplimit)
    {
        set_cur = rand()% g_vrecset.size();
        rec_cur = rand()% (g_vrecset[set_cur]).size();
        it = (g_vrecset[set_cur]).begin() + rec_cur ;
        if (rec_cut(*it,rec1,rec2))
        {
            g_vrecset[set_cur].erase(it);
            g_vrecset[set_cur].push_back(rec1);
            g_vrecset[set_cur].push_back(rec2);
        }
    }
}

bool rec_cut(const rectangle rec, rectangle & rec1 ,rectangle & rec2)
{
    int k = 0;
    // 分割宽
    if (rand()%2 == 0)
    {
        k = rand()% rec.width;
        if (k >= 1 && rec.width -k >= 1)
        {
            rec1.width = k;
            rec2.width = rec.width - k;
            rec1.height = rec2.height = rec.height ;
            rec1.time = rec2.time = 1;
        }
        else
            return 0;
    }
    else // 分割高
    {
        k = rand() % rec.height;
        if (k >= 1 && rec.height -k >= 1)
        {
            rec1.height = k;
            rec2.height = rec.height - k;
            rec1.width = rec2.width = rec.width;
            rec1.time = rec2.time = 1;
        }
        else
            return 0;
    }
    return 1;
}
    

void rec_merge()
{
    vector<rectangle> ::iterator itvec;
    for (vector< vector<rectangle>  > :: iterator it = g_vrecset.begin() ;
         it != g_vrecset.end(); ++it)
    {
        for (vector<rectangle>::iterator itv = it->begin();
             itv != it->end() ; ++itv)
        {
            for (vector< vector<rectangle>  >::iterator itset= g_vrecset.begin();
                 itset != g_vrecset.end() && itset != it; ++itset)
            {
                itvec = find(itset->begin(), itset->end() , *itv );
                if (itvec != itset->end())
                {
                    itv->time++;
                    itset->erase(itvec);
                    break;
                }
            }
            
        }
    }
}

void output_data()
{
    
    ofstream ofile("data.txt");
    ofile<<g_width<<" "<<g_height<<" "<<g_opt_time<<" "<<endl;
    ofile<<g_rec.size()<<endl;
    for (vector<rectangle>::iterator it = g_rec.begin(); it != g_rec.end(); ++it)
        ofile<<it->width<<" "<<it->height<<" "<<it->time<<endl;
}

bool check()
{
    bool error_mark = 1;
    int number = 0;
    int area = 0;
    int time_sum = 0;
    
    for (vector< vector<rectangle>  > :: iterator it = g_vrecset.begin() ;
         it != g_vrecset.end(); ++it)
    {
        number += it->size();
        for (vector<rectangle>::iterator itv = it->begin();
             itv != it->end() ; ++itv)
        {
            time_sum += itv->time;
            area += itv->width * itv->height * itv->time;
        }
        
    }
    if (time_sum != g_rec_number_uplimit)
    {
        cout<<"rec number error"<<endl;
        error_mark = 0;
    }
    if (area != g_width * g_height * g_opt_time)
    {
        cout<<"area error"<<endl;
        error_mark = 0;
    }
    if (error_mark == 0)
    {
        cout<<" 矩形块个数:"<<number<<endl;
        cout<<" 合并前个数:"<<time_sum<<endl;
        cout<<"合并前理论个数："<< g_rec_number_uplimit<<endl;

        cout<<"理论面积和:"<<g_width * g_height * g_opt_time<<endl;
        cout<<"实际面积和:"<<area<<endl;
    }
    
    return error_mark;
}


void push_data2vec()
{
    for (vector< vector<rectangle>  > :: iterator it = g_vrecset.begin() ;
         it != g_vrecset.end(); ++it)
    {
        for (vector<rectangle>::iterator itv = it->begin();
             itv != it->end() ; ++itv)
            g_rec.push_back(*itv);
    }
}

void shullfle_datas()
{
    random_shuffle(g_rec.begin(),g_rec.end());
}
