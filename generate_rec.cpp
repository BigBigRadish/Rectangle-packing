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


void rec_sub();

void init();

int g_width;
int g_height;
int g_opt_time;

int g_rec_number_uplimit;



void rec_merge();
bool rec_cut(const rectangle rec, rectangle & rec1 ,rectangle & rec2);


void output_data();

int get_rec_number();



int main(int arg ,char *arv[])
{

    init();
    rec_sub();
    cout<<get_rec_number()<<endl;
    rec_merge();
    output_data();
    return 0;
            
    
}


void init()
{
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
    ofile<<g_width<<" "<<g_height<<endl;
    ofile<<get_rec_number()<<endl;
    for (vector< vector<rectangle>  > :: iterator it = g_vrecset.begin() ;
         it != g_vrecset.end(); ++it)
    {
        for (vector<rectangle>::iterator itv = it->begin();
             itv != it->end() ; ++itv)
            ofile<<itv->width<<"  "<<itv->height<<"  "<<itv->time<<endl;
    }
}
