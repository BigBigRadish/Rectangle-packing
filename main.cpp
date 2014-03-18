/*************************************************************
*
*main.cpp: the main cpp
*auther     : caowg
*Written on : 03/17/14.
*
*************************************************************/

#include <iostream>
#include "rec_packing.H"
#include <set>

using namespace std;

vector<Hline> g_s_hline; // 所有水平线


bool chose_as_rec(vector<rectangle> & v_rec,vector<action_space> & v_as,
                  vector<rectangle>::iterator i2chonse_rec,
                  vector<action_space>::iterator i2chonse_as);

void update_action_space(vector<rectangle>::iterator i2chonse_rec
                         vector<action_space>::iterator i2chonse_as,
                         vector<action_space> & v_as,
                         vector<rectangle> & v_rec);

// 计算穴度
bool calculate_fd(vector<rectangle>:: iterator i2rec,
                  vector<action_space>::iterator i2as,
                  fit_degree & fd);




int main(int arg ,char *arv[])
{
        
}


bool chose_as_rec(vector<rectangle> & v_rec,vector<action_space> & v_as,
                  vector<rectangle>::iterator i2chonse_rec,
                  vector<action_space>::iterator i2chonse_as)
{
    vector<rectangle>::iterator i2rec = v_rec.begin();
    vector<action_space>::iterator i2as = v_as.begin();
    vector<rectangle>::iterator i2rec_chosen = v_rec.begin();
    vector<action_space>::iterator i2as_chosen = v_as.begin();

    fit_degree fd;
    fit_degree last_fd;
    
    
    for (; i2as!= v_as.end() ; i2rec++) // 迭代动作空间
    {
        for (; i2rec != v_rec.end() ; ++i2rec) // 迭代外部矩形块
        {
            calculate_fd(i2rec,i2as,fd);
            if(last_fd < fd || (fd == last_fd && *i2rec > *i2rec_chosen) )
            {
                i2rec_chosen = i2rec;
                i2as_chosen = i2as ;
                last_fd = fd;
            }
        }
    }
}

// 计算贴边数
int calculate_fd_k(vector<rectangle>::iterator i2rec,
                          vector<action_space>::iterator i2as)
{
    int fd_k = 0 ;
    if(i2rec->get_width() == i2as->get_width() )
        fd_k += 2;
    else
        fd_k += 1 ;
    if(i2rec->get_height() == i2as->get_height() )
        fd_k += 2;
    else
        fd_k += 1 ;
    return fd_k ;
}

// 计算平整度
// 没有采用剩余动作个数
double calculate_fd_s(vector<rectangle>::iterator i2rec,
                          vector<action_space>::iterator i2as)
{
    double di = 0 ;
    di = min(i2as->get_width - i2rec->get_width,i2as->get_height - i2rec->get_height);
    return di/(i2rec->get_width + i2rec->get_height);
}


// 计算它贴边数
int  calculate_fd_p(vector<rectangle>::iterator i2rec,
                          vector<action_space>::iterator i2as)
{
    int fd_p = 0;
    Hline top_line(i2rec->left_top,i2rec->right_top);
    Hline bottle_line(i2rec->left_bottle,i2rec->right_bottle);
    Vline left_line(i2rec->left_bottle,i2rec->left_top);
    Vline left_line(i2rec->right_bottle,i2rec->right_top);
    
    vector<Hline>::iterator ihline;
    vector<Vline>::iterator ivline;
    for (ihline = g_s_hline.begin(); ihline != g_s_hline.end(); ++ihline)
    {
        if(top_line.conflict(*ihline))
            fd_p++;
        if (bottle_line.conflict(*ihline))
            fd_p++;
    }

    for (ivline = g_s_vline.begin(); ivline != g_s_vline.end(); ++ivline)
    {
        if(left_line.conflict(*ivline))
            fd_p++;
        if (right_line.conflict(*ivline))
            fd_p++;
    }
    return fd_p;
}



// 计算小矩形在动作空间 i2as 四个角的fd，包括水平和垂直方向
// 也即有4*2 个值选取
bool calculate_fd(vector<rectangle>:: iterator i2rec,
                  vector<action_space>::iterator i2as,
                  fit_degree & fd)
{
    // 放不下,则直接返回false
    if(i2rec->get_width() > i2as->get_width() ||
        i2rec->get_height() > i2as->get_height() )
        return 0;

    fd.k = calculate_fd_k(i2rec,i2as);
    fd.s = calculate_fd_s(i2rec,i2as);
    fd.p = calculate_fd_p(i2rec,i2as);
    
}
