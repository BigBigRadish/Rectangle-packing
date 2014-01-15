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
#include <algorithm>
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

void conner::update_conner_type(const rectangle& _rec_block)
{
    // 动作空间的角被放入的新矩形块 _rec 覆盖
    if((x <= _rec_block.x + _rec_block.width )
       && ( y<= _rec_block.y+ _rec_block.height) )
    {
        ctype = 1;
        break;
    }
}

// begin class conner

void conner::update_conner_type(const rectangle& _rec_block)
{
    // 动作空间的角被放入的新矩形块 _rec 覆盖
    if((x <= _rec_block.x + _rec_block.width )
       && ( y<= _rec_block.y+ _rec_block.height) )
    {
        is_valid = 1;
        break;
    }
}

// end class conner


// begin class rectangle
void rectangle:: set_rectangle(int _lb_x, int _lb_y,int _width, int _height )
{
    left_bottle.x = _lb_x;
    left_bottle.y = _lb_y;
    right_top.x = _lb_x + _width;
    right_top.y = _lb_y + _height;
}


// end class rectangle


// begin class rec_cinflict 

void rec_conflict::operator() (action_space & ac_space)
{
    int minx = max(ac_space.left_bottle.x , rec_block.left_bottle.x);
    int maxx = min(ac_space.right_top.x , rec_block.right_top.x );
    int miny = max(ac_space.left_bottle.y, rec_block.left_bottle.y);
    int maxy = min(ac_space.right_top.y , rec_block.right_top.y);

    if( minx >= maxx || miny >= maxy)// not conflict
        ac_space.is_conflict = 0;
    else                          // action space ac_space conflict 
        ac_space.is_conflict = 1; // with rec_block                 
}



void rec_conflict::set_rectangle(int _lb_x, int _lb_y,int _width, int _height )
{
    rec_block.set_rectangle(_lb_x,_lb_y,_width,_height);
}
