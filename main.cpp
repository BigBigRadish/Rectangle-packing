/*************************************************************
*
*main.cpp: the main cpp
*auther     : caowg
*Written on : 03/17/14.
*
*************************************************************/

#include <iostream>
#include "rec_packing.H"

using namespace std;

vector<Hline> g_v_hline; // 所有水平线
vector<Vline> g_v_vline ; // 所有垂直线
vector<rectangle> g_v_rec_undo ; // 未处理的木块
vector<rectangle> g_v_rec_done ; // 已经处理的小木块
vector<action_space> g_v_as ; // 动作空间

// 选择当前最优的放置
bool chose_as_rec(vector<rectangle>::iterator & i2chonse_rec,
                  vector<action_space>::iterator & i2chonse_as);

// 更新动作空间
void update_action_space(vector<rectangle>::iterator i2chonse_rec
                         vector<action_space>::iterator i2chonse_as );

// 更新线段
void update_line(vector<rectangle>::iterator i2chonse_rec);



// 计算穴度
// 确保不该边 i2rec 对应的值
void calculate_fd(vector<rectangle>:: const_iterator i2rec,
                  vector<action_space>::const_iterator i2as,
                  fit_degree & fd);




int main(int arg ,char *arv[])
{
        
}

// 选择动作空间和小木块
bool chose_as_rec(vector<rectangle>::iterator & i2chonse_rec,
                  vector<action_space>::iterator & i2chonse_as)
{
    if (g_v_rec_undo.size() == 0)
        return false;
    
    vector<rectangle>::iterator i2rec = g_v_rec_undo.begin();
    vector<action_space>::iterator i2as = g_v_as.begin();
    vector<rectangle>::iterator i2rec_chosen = g_v_rec_undo.begin();
    vector<action_space>::iterator i2as_chosen = g_v_as.begin();

    rectangle rec_chosen = g_v_rec_undo[0];
    fit_degree fd;
    fit_degree max_fd ;
    bool finded = 0;
    
    for (; i2as!= g_v_as.end() ; i2rec++) // 迭代动作空间
    {
        for (; i2rec != g_v_rec_undo.end() ; ++i2rec) // 迭代外部矩形块
        {
            if(!max_fd_of8values(i2rec,i2as,fd))
                continue;
            finded = 1;
            if(max_fd < fd || (fd == max_fd && *i2rec > rec_chosen) )
            {
                rec_chosen = *i2rec ;  // 由于小方块在迭代过程中会被改变，所以保存当前最优解的值
                                        // 坐标，放置方式等会改变
                i2rec_chosen = i2rec; // 指向当前最优解的那个小方块
                i2as_chosen = i2as ;
                max_fd = fd;
            }
        }
    }
    if (finded)
    {
        *i2rec_chosen  = rec_chosen ; // 将最优解的那个小方块设置为其最优值
        i2as->place_type = i2rec_chosen->place_type;
    }
    return finded ;
}

// 计算贴边数
int calculate_fd_k(vector<rectangle>::const_iterator i2rec,
                          vector<action_space>::const_iterator i2as)
{
    int fd_k = 0 ;
    if(i2rec->width == i2as->width )
        fd_k += 2;
    else
        fd_k += 1 ;
    if(i2rec->height == i2as->height )
        fd_k += 2;
    else
        fd_k += 1 ;
    return fd_k ;
}

// 计算平整度
// 没有采用剩余动作个数
double calculate_fd_s(vector<rectangle>::const_iterator i2rec,
                          vector<action_space>::const_iterator i2as)
{
    double di = 0 ;
    di = min(i2as->get_width - i2rec->get_width,i2as->get_height - i2rec->get_height);
    return di/(i2rec->get_width + i2rec->get_height);
}


// 计算它贴边数
int  calculate_fd_p(vector<rectangle>::const_iterator i2rec,
                          vector<action_space>::const_iterator i2as)
{
    int fd_p = 0;
    Hline top_line(i2rec->left_top(),i2rec->right_top());
    Hline bottle_line(i2rec->left_bottle,i2rec->right_bottle());
    Vline left_line(i2rec->left_bottle,i2rec->left_top());
    Vline left_line(i2rec->right_bottle(),i2rec->right_top());
    
    vector<Hline>::iterator ihline;
    vector<Vline>::iterator ivline;
    for (ihline = g_v_hline.begin(); ihline != g_v_hline.end(); ++ihline)
    {
        if(top_line.conflict(*ihline))
            fd_p++;
        if (bottle_line.conflict(*ihline))
            fd_p++;
    }

    for (ivline = g_v_vline.begin(); ivline != g_v_vline.end(); ++ivline)
    {
        if(left_line.conflict(*ivline))
            fd_p++;
        if (right_line.conflict(*ivline))
            fd_p++;
    }
    return fd_p;
}



// 计算当前放置下的fd
void calculate_fd(vector<rectangle>:: const_iterator i2rec,
                  vector<action_space>::const_iterator i2as,
                  fit_degree & fd)
{
    fd.k = calculate_fd_k(i2rec,i2as);
    fd.s = calculate_fd_s(i2rec,i2as);
    fd.p = calculate_fd_p(i2rec,i2as);
}

// 求出木块 i2rec 在动作空间i2as的4个位置中最忧位置
// 并且 i2rec 的值已经被更新
// 4 个位置是因为没有旋转
void  max_fd_of4values(const vector<rectangle>:: iterator & i2rec,
            const vector<action_space>::iterator & i2as,
            fit_degree & fd)
{
    fit_degree fd_max ; // 本动作空间针对当前木块目前最大的fd
    rectangle rec_op = *i2rec; // 当前最优的小木块

    // left bottle
    i2rec->set_ordinate_lb(i2as->left_bottle);
    calculate_fd(i2rec,i2as,fd_max);
    i2as->place_type = LEFT_BOTTLE ;
    rec_op = *i2rec;

    // left top
    i2rec->set_ordinate_lt(i2as->left_top() );
    calculate_fd(i2rec,i2as,fd);
    if (fd_max < fd || (fd_max == fd &&  rec_op < *i2rec ) )
    {
        fd_max = fd ;
        rec_op = *i2rec;
        i2as->place_type = LEFT_TOP ;
    }
        
    // right bottle
    i2rec->set_ordinate_rb(i2as->right_bottle() );
    calculate_fd(i2rec,i2as,fd);
    if (fd_max < fd || (fd_max == fd &&  rec_op < *i2rec ) )
    {
        fd_max = fd ;
        rec_op = *i2rec;
        i2as->place_type = RIGHT_BOTTLE ;
    }

    // right top
    i2rec->set_ordinate_rt(i2as->right_top() );
    calculate_fd(i2rec,i2as,fd);
    if (fd_max < fd || (fd_max == fd &&  rec_op < *i2rec ) )
    {
        fd_max = fd ;
        rec_op = *i2rec;
        i2as->place_type = RIGHT_TOP ;
    }
    // 根据放置方式，重新定小方块的坐标
    switch( i2as->place_type)
    {
    case LEFT_BOTTLE:
        i2rec->set_ordinate_lb(i2as->left_bottle);break;
    case LEFT_TOP:
        i2rec->set_ordinate_lt(i2as->left_top() );break;
    case RIGHT_BOTTLE:
        i2rec->set_ordinate_rb(i2as->right_bottle() );break;
    case RIGHT_TOP:
        i2rec->set_ordinate_rt(i2as->right_top() );break;
    default:
        cout<<"error";
    }
}

// 计算小矩形在动作空间 i2as 四个角的fd，包括水平和垂直方向
// 也即有4*2 个值选取
bool max_fd_of8values(const vector<rectangle>:: iterator & i2rec,
                      const vector<action_space>::iterator & i2as,
            fit_degree & fd)
{
    // 放不下,则直接返回false
    if(i2rec->width > i2as->width ||
        i2rec->height > i2as->height )
        return 0;
    
    max_fd_of4values(i2rec , i2as, fd);
    i2rec->rec_reverse(); // 宽高互换
    fit_degree fd_reverse ;
    rectangle rec_un_reverse(*i2rec);
    max_fd_of4values(i2rec , i2as, fd_reverse);
    if (fd < fd_reverse ) // 如果reverse后和之前 fd相同，则优先考虑未reverse的情况
    {
        fd = fd_reverse ;
    }
    else
    {
        i2rec->rec_reverse(); // 重新恢复转置
        *i2rec = rec_un_reverse;
        i2as->place_type = i2rec->place_type;
    }
    return 1 ;
}



void update_action_space(vector<rectangle>::iterator i2chonse_rec
                         vector<action_space>::iterator i2chonse_as )
{
    
}
