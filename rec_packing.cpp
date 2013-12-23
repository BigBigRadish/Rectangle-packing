/*************************************************************
*
*rec_packing.cpp: the rectangle packing
*auther     : caowg
*Written on : 07/31/13.
*
*************************************************************/

#include <iostream>
#include <set>
#include <vector>

#include "rec_packing.H"

using namespace std;



set<Hline, std::less<Hline> > lbset; // 左下角对应的下沿线
set<Hline, std::greater<Hline> > ltset; // 左上角对应的上沿线


typedef set<Hline, std::less<Hline> > :: iterator lbiterator; // 下沿线对应的迭代起
typedef set<Hline, std::greater<Hline> > :: iterator ltiterator; // shang沿线对应的迭代起

lbiterator lbitera;
ltiterator ltitera;

vector<int> ivec;

// 判断角是否为实角
bool is_valid_conner(const Conner& conner );

// 判断两个矩形是否有重合部分
void  rec_conflict();




int main(int arg ,char *arv[])
{
    Hline l1;
    ltset.insert(l1);
    Hline l2(1,1,1);
    ltset.insert(l2);

    
    
    
}

bool _lblineIsblock( Hilne & hline ,Conner cner)
{
    for(lbitera = lbset.begin(); lbitera != lbset.find(hline) ;lbitera++ )
    {
        if( lbitera->x_left < hilne.x_left && lbitera->x_right > cner.x )
            return 1;
    }
    for (ltitera  = ltset.rbegin();  ltitera != ltset.rend() ; ltitera++)
    {
        if(ltitera->y >= hline.y) // 在线段之上
            break;
        if(ltitera->y <= cner.y || ltitera->x_left > hline.x_left) // 在角之下 || 在直线右边
            continue;
        if(ltitera->x_right > cner.x) // 屏蔽线段 hline
            return 1;
    }
    return 0;
    
}

inline bool _lblineIsvalid(const lbiterator & hline , const Coner & cner)
{
    if(hline->y > cner.y && hline->x_right > cner.x)
        return 0;
    return 1;
}

// 满足 _lblineIsblock 为true 为前提
inline bool _lbline_is_block_others(const lbiterator & hline)
{
    if(hline->x_left <= cner.x)
        return 1;
}

void update_lb_line(const lbiterator & hline)
{
    lbitera = lbset.find(hline);
    if(lbitera == lbset.end())
        lbset.insert(*hline);
    else
    {
        
    }
    
}

void conner::set_conner(int _x, int _y)
{
    x = _x;
    y = _y;
}

void conner::update_conner_type(const vector<rectangle> & vec_rec)
{
    vector<rectangle>::it;
    for (it = vec_rec.begin(); it != vec_rec.end() ;it++)
    {
        if((x <= it->left_bottle.x + it->width )
           && ( y<= it->left_bottle.y+ it->height) )
        {
            ctype = 1;
            break;
        }
    }
        
}

void rectangle:: set_rectangle(int _lb_x, int _lb_y,int _width, int _height )
{
    left_bottle.set_conner(_lb_x,_lb_y);
    width = _width;
    height = _height;
}

void rec_conflict::operator() (action_space & ac_space)
{
    
        
}

void rec_conflict::set_rectangle(int _lb_x, int _lb_y,int _width, int _height )
{
    rec_block.set_rectangle(_lb_x,_lb_y,_width,_height);
}
