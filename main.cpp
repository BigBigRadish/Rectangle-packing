/*************************************************************
*
*main.cpp: the main cpp
*auther     : caowg
*Written on : 03/17/14.
*
*************************************************************/

#include <iostream>
#include <fstream>
#include "rec_packing.H"
#include <string>

using namespace std;



vector<Hline> g_v_hline; // 所有水平线
vector<Vline> g_v_vline ; // 所有垂直线
vector<Hline> g_v_hline_backup; // 所有水平线
vector<Vline> g_v_vline_backup ; // 所有垂直线

vector<rectangle> g_v_rec_undo ; // 未处理的小矩形
vector<rectangle> g_v_rec_done ; // 已经处理的小矩形



vector<action_space> g_v_as_backup; // 因为要计算平整度，所以备份动作空间
vector<action_space> g_v_as ; // 动作空间
vector<action_space> g_v_as_conflict ;// 保存被放入矩形影响的动作空间




set<conner> g_s_conner; // 所有的实角
set<conner> g_s_conner_backup; // 平整度备份

set<conner> g_s_conner_blocked; // 记录当前被屏蔽的角
set<conner> g_s_conner2space; // 每次新生成的角,新动作空间从这些角产生


action_space g_as(conner(0,0,0),0,0);



const int MAX = 999999;
const int MIN = -99999;

void print_data();



// 选择当前最优的放置
bool chose_as_rec(vector<rectangle>::iterator & i2chonse_rec,
                  vector<action_space>::iterator & i2chonse_as);

// 更新动作空间
void update_action_space(vector<rectangle>::iterator i2chonse_rec,
                         vector<action_space>::iterator i2chonse_as );

// 计算穴度
// 确保不该边 i2rec 对应的值
void calculate_fd(vector<rectangle>:: iterator i2rec,
                  vector<action_space>::iterator i2as,
                  fit_degree & fd);

// 计算贴边数
int calculate_fd_k(vector<rectangle>::iterator i2rec,
                   vector<action_space>::iterator i2as);


// 计算平整度
// 没有采用剩余动作个数
int calculate_fd_s(vector<rectangle>::iterator i2rec,
                      vector<action_space>::iterator i2as);


// 求出木块 i2rec 在动作空间i2as的4个位置中最忧位置
// 并且 i2rec 的值已经被更新
// 4 个位置是因为没有旋转
void  max_fd_of4values(const vector<rectangle>:: iterator & i2rec,
                       const vector<action_space>::iterator & i2as,
                       fit_degree & fd);

// 计算小矩形在动作空间 i2as 四个角的fd，包括水平和垂直方向
// 也即有4*2 个值选取
bool max_fd_of8values(const vector<rectangle>:: iterator & i2rec,
                      const vector<action_space>::iterator & i2as,
                      fit_degree & fd);

// 左下角算法,由左下角生成动作空间
void conner2as_lb(const conner & lb_conner);

// 左上角算法
void conner2as_lt(const conner & lt_conner);

// 右上角算法
void conner2as_rt(const conner & rt_conner);

// 右下角算法
void conner2as_rb(const conner & rb_conner);

// 寻找小矩形的下沿线和垂直线构成的角
void find_conner_vline2downline(const Vline & vline, const Hline & down_line);

// 寻找小矩形的上沿线和垂直线构成的角
void find_conner_vline2upline(const Vline & vline, const Hline & up_line);

// 寻找小矩形的左沿线和水平线构成的角
void find_conner_hline2leftline(const Hline & hline, const Vline & left_line);

// 寻找小矩形的右沿线和水平线构成的角
void find_conner_hline2rightline(const Hline & hline, const Vline & right_line);


// 生成实角
void generate_conners(vector<rectangle>::iterator i2chonse_rec);

// 寻找被矩形块 i2chonse_rec冲突的动作空间
// 并且把这几个动作空间的实角添加到 生成动作空间的角集合中
void find_conflict_as(vector<rectangle>::iterator i2chonse_rec);

//
bool is_conflicted(const action_space& as);

// 更新动作空间
void update_action_space();

void deal();




    

    
    

// 初始化操作
void init();



// 输出最后数据
void output_data();



int main(int arg ,char *arv[])
{
    vector<int> iv;
    iv.reserve(100);
    vector<int> iv2;
    iv2.reserve(10);
    iv.push_back(1);
    iv2.push_back(2);
    cout<<&iv[0]<<endl;
    
    iv = iv2;
    cout<<&iv[0]<<endl;
    
    
    init();
    
    deal();
}

void init()
{
    // 读入矩形框，以及木块
    ifstream ifile;
    ifile.open("data.txt");
    int rec_number;
    ifile >> g_as.width;
    ifile >> g_as.height;
    cout<<"the big as:"<<g_as.width<<" "<<g_as.height<<endl;
    g_as.left_bottle.x = 0 ;
    g_as.left_bottle.y = 0 ;
    ifile >> rec_number;
    rectangle rec;
    int i = 0 ;
    while((!ifile.eof()) && i< rec_number)
    {
        i++;
        ifile >> rec.width;
        ifile >> rec.height;
        cout<<"read:"<<rec.width<<" "<<rec.height<<endl;
        g_v_rec_undo.push_back(rec);
    }
    cout<<"rec size:"<<g_v_rec_undo.size()<<endl;


    // 预分配空间
    // 否则会导致 chose_as_rec 迭代时候，迭代器失效，因为
    g_v_as.reserve(1000);
    g_v_as_backup.reserve(1000);

    // 初始化动作空间
    g_v_as.push_back(g_as);
    
    
    // 初始化角
    g_s_conner.insert(conner(g_as.left_bottle.x,g_as.left_bottle.y,1,LEFT_BOTTLE) );
    g_s_conner.insert(conner(g_as.left_top().x,g_as.left_top().y,1,LEFT_TOP ));
    g_s_conner.insert(conner(g_as.right_top().x,g_as.right_top().y,1,RIGHT_TOP ));
    g_s_conner.insert(conner(g_as.right_bottle().x,g_as.right_bottle().y,1,RIGHT_BOTTLE ));

    // 初始化线
    g_v_hline.push_back(Hline(g_as.left_top(),g_as.right_top(),DOWN_LINE));
    g_v_hline.push_back(Hline(g_as.left_bottle,g_as.right_bottle(),UP_LINE));
    g_v_vline.push_back(Vline(g_as.left_bottle,g_as.left_top(),RIGHT_LINE));
    g_v_vline.push_back(Vline(g_as.right_bottle(),g_as.right_top(), LEFT_LINE));
    
}

// 消除被覆盖的实角
void remove_conner_blocked(rectangle & rec)
{
    g_s_conner_blocked.clear();
    set<conner>::iterator it = g_s_conner.end();
    // 被屏蔽的角保存下来，生成角的时候用来再次屏蔽
    if ((it = g_s_conner.find(rec.left_top()) )!= g_s_conner.end() )
    {
        g_s_conner_blocked.insert(*it);
        g_s_conner.erase(it);
    }
    if ((it = g_s_conner.find(rec.right_top()) )!= g_s_conner.end() )
    {
        g_s_conner_blocked.insert(*it);
        g_s_conner.erase(it);
    }
    if ((it = g_s_conner.find(rec.left_bottle) )!= g_s_conner.end() )
    {
        g_s_conner_blocked.insert(*it);
        g_s_conner.erase(it);
    }
    if ((it = g_s_conner.find(rec.right_bottle()) )!= g_s_conner.end() )
    {
        g_s_conner_blocked.insert(*it);
        g_s_conner.erase(it);
    }

    g_s_conner2space.erase(rec.left_top());
    g_s_conner2space.erase(rec.right_top());
    g_s_conner2space.erase(rec.left_bottle);
    g_s_conner2space.erase(rec.right_bottle());
}


void deal()
{
    vector<rectangle>::iterator i2chonse_rec = g_v_rec_undo.begin();
    vector<action_space>::iterator i2chonse_as = g_v_as.begin();
    rectangle rec_chonse ;
    action_space as_chonse;
    
    while(chose_as_rec(i2chonse_rec,i2chonse_as))
    {
        // vector会改变，所以迭代器有可能失效，需要保存其对应值
        cout<<"begin this chonsen:"<<endl;
        print_data();

        cout<<"chonse it"<<i2chonse_rec->width<<" "<<i2chonse_rec->height<<endl;
        
        rec_chonse = *i2chonse_rec;
        as_chonse = *i2chonse_as ;
        
        find_conflict_as(i2chonse_rec);
        remove_conner_blocked(rec_chonse);
        generate_conners(i2chonse_rec);
        update_action_space();

        g_v_rec_done.push_back(*i2chonse_rec);
        g_v_rec_undo.erase(i2chonse_rec);

        // test info
        cout<<"end this chonsen:"<<endl;
        
        print_data();

    }
    output_data();
}

// 选择动作空间和小木块
bool chose_as_rec(vector<rectangle>::iterator & i2chonse_rec,
                  vector<action_space>::iterator & i2chonse_as)
{
    cout<<"chose_as_rec"<<endl;
    if (g_v_rec_undo.size() == 0)
        return false;
    
    vector<rectangle>::iterator i2rec = g_v_rec_undo.begin();
    vector<action_space>::iterator i2as = g_v_as.begin();
    // vector<rectangle>::iterator i2rec_chosen = g_v_rec_undo.begin();
    // vector<action_space>::iterator i2as_chosen = g_v_as.begin();

    rectangle rec_chosen = g_v_rec_undo[0];
    fit_degree fd;
    fit_degree max_fd ;
    bool finded = 0;
    
    for (; i2as!= g_v_as.end() ; i2as++) // 迭代动作空间
    {
        for (; i2rec != g_v_rec_undo.end() ; ++i2rec) // 迭代外部矩形块
        {
            cout<<"begin test rec:"<<i2rec->width<<" "<<i2rec->height<<
                " as:"<<i2as->width<<" "<<i2as->height<<" as size"<<g_v_as.size();
            if(!max_fd_of8values(i2rec,i2as,fd))
                continue;
            finded = 1;
            cout<<" fd.k"<<fd.k<<endl;
            if(max_fd < fd || (fd == max_fd && *i2rec > rec_chosen) )
            {
                rec_chosen = *i2rec ;  // 由于小方块在迭代过程中会被改变，所以保存当前最优解的值
                                        // 坐标，放置方式等会改变
                i2chonse_rec = i2rec; // 指向当前最优解的那个小方块
                i2chonse_as = i2as ;
                max_fd = fd;
                cout<<"max_fd.k "<<max_fd.k<<"  "<<i2rec->width<<" "<<i2rec->height<<endl;
            }
        }
    }
    if (finded)
    {
        *i2chonse_rec  = rec_chosen ; // 将最优解的那个小方块设置为其最优值
        i2chonse_as->place_type = i2chonse_rec->place_type;
    }
    cout<<"return :"<<rec_chosen.width<<" "<<rec_chosen.height<<endl;
    
    return finded ;
}

// 计算贴边数
int calculate_fd_k(vector<rectangle>::iterator i2rec,
                          vector<action_space>::iterator i2as)
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
int calculate_fd_s(vector<rectangle>::iterator i2rec,
                      vector<action_space>::iterator i2as)
{
    rectangle rec_chonse;
    action_space as_chonse;
    cout<<"fd_s:width:"<<i2as->width<<endl;
    // backup
    g_s_conner_backup = g_s_conner;
    g_v_as_backup = g_v_as;
    // g_v_as_backup.clear();
    
    g_v_hline_backup = g_v_hline;
    g_v_vline_backup = g_v_vline;

    rec_chonse = *i2rec;
    as_chonse = *i2as;
    find_conflict_as(i2rec);
    remove_conner_blocked(rec_chonse);
    generate_conners(i2rec);
    update_action_space();
    
    int s = g_v_as.size();

    // restor
    g_v_as = g_v_as_backup;
    cout<<g_v_as[0].width<<endl;
    cout<<"i2as:"<<i2as->width<<endl;
    
    g_s_conner = g_s_conner_backup;
    g_v_hline = g_v_hline_backup;
    g_v_vline = g_v_vline_backup;

    return s;
}


// 计算它贴边数
int  calculate_fd_p(vector<rectangle>::iterator i2rec,
                          vector<action_space>::iterator i2as)
{
    int fd_p = 0;
    Hline top_line(i2rec->left_top(),i2rec->right_top(),UP_LINE);
    Hline bottle_line(i2rec->left_bottle,i2rec->right_bottle(),DOWN_LINE);
    Vline left_line(i2rec->left_bottle,i2rec->left_top(),LEFT_LINE);
    Vline right_line(i2rec->right_bottle(),i2rec->right_top(),RIGHT_LINE);
    
    vector<Hline>::iterator ihline;
    vector<Vline>::iterator ivline;
    int topline_mark = 0;
    int bottleline_mark = 0 ;
    int leftline_mark = 0;
    int rightline_mark = 0;
    for (ihline = g_v_hline.begin(); ihline != g_v_hline.end(); ++ihline)
    {
        if(top_line.conflict(*ihline) && topline_mark == 0)
        {
            fd_p++;
            topline_mark = 0;
        }
        
        if (bottle_line.conflict(*ihline) && bottleline_mark == 0)
        {
            fd_p++;
            bottleline_mark = 0 ;
        }
        
    }

    for (ivline = g_v_vline.begin(); ivline != g_v_vline.end(); ++ivline)
    {
        if(left_line.conflict(*ivline) && leftline_mark == 0)
        {
            fd_p++;
            leftline_mark = 0 ;
        }
        
        if (right_line.conflict(*ivline) && rightline_mark == 0)
        {
            fd_p++;
            rightline_mark == 0;
        }
        
    }
    return fd_p;
}



// 计算当前放置下的fd
void calculate_fd(vector<rectangle>:: iterator i2rec,
                  vector<action_space>::iterator i2as,
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

    // left bottle 是实角
    if(g_s_conner.count(i2as->left_bottle))
    {
        i2rec->set_ordinate_lb(i2as->left_bottle);
        calculate_fd(i2rec,i2as,fd_max);
        i2as->place_type = LEFT_BOTTLE ;
        rec_op = *i2rec;
    }

    // left top 是实角
    if (g_s_conner.count(i2as->left_top()))
    {
        i2rec->set_ordinate_lt(i2as->left_top() );
        cout<<"before cal"<<i2as->width<<endl;
        calculate_fd(i2rec,i2as,fd);
        cout<<"after cal"<<i2as->width<<endl;
        if (fd_max < fd || (fd_max == fd &&   *i2rec > rec_op) )
        {
            fd_max = fd ;
            rec_op = *i2rec;
            i2as->place_type = LEFT_TOP ;
        }   
    }
        
    // right bottle 是实角
    if (g_s_conner.count(i2as->right_bottle()))
    {
        i2rec->set_ordinate_rb(i2as->right_bottle() );
        calculate_fd(i2rec,i2as,fd);
        cout<<"after cal"<<i2as->width<<endl;
        if (fd_max < fd || (fd_max == fd &&  *i2rec > rec_op ) )
        {
            fd_max = fd ;
            rec_op = *i2rec;
            i2as->place_type = RIGHT_BOTTLE ;
        }
    }

    // right top 是 实角
    if (g_s_conner.count(i2as->right_top()))
    {
        i2rec->set_ordinate_rt(i2as->right_top() );
        calculate_fd(i2rec,i2as,fd);
        cout<<"after cal"<<i2as->width<<endl;
        if (fd_max < fd || (fd_max == fd &&   *i2rec > rec_op) )
        {
            fd_max = fd ;
            rec_op = *i2rec;
            i2as->place_type = RIGHT_TOP ;
        }
    }
    fd_max = fd;
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
    cout<<"after 4value"<<i2as->width<<endl;

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
    rectangle rec_un_reverse(*i2rec);
    i2rec->rec_reverse(); // 宽高互换
    fit_degree fd_reverse ;
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
    cout<<"this test over: as width:"<<i2as->width<<endl;

    return 1 ;
    
}


// 左下角算法
void conner2as_lb(const conner & lb_conner)
{
    
    
    //a1 左下角往上，找到新动作右上角y坐标
    int as_top_y = MAX;
    for (vector<Hline>::iterator it = g_v_hline.begin() ;  it != g_v_hline.end(); ++it)
    {
        // 寻找满足条件的y坐标的最小的
        if (it->pt_left.x <= lb_conner.x && it->pt_right.x > lb_conner.x
            && it->get_y() > lb_conner.y)
        {
            if(as_top_y > it->get_y() )
                as_top_y = it->get_y();
        }
    }
    
    //a2 寻找右上角的x坐标
    int as_right_x = MAX ;
    for (vector<Vline>::iterator it = g_v_vline.begin() ; it != g_v_vline.end() ; it++)
    {
        // 寻找满足条件的x坐标最小的
        if (it->get_x() > lb_conner.x && it->pt_top.y > lb_conner.y
            && it->pt_bottle.y < as_top_y)
        {
            if (as_right_x > it->get_x())
                as_right_x = it->get_x();
        }
    }
    action_space as(lb_conner,as_right_x - lb_conner.x, as_top_y - lb_conner.y);
    if (find(g_v_as.begin(), g_v_as.end(), as) == g_v_as.end())
        g_v_as.push_back(as);
    
    //b1  左下角往上，寻找右上角的x坐标
    as_right_x = MAX ;
    as_top_y = MAX ;
    for (vector<Vline>::iterator it = g_v_vline.begin() ; it != g_v_vline.end() ; it++)
    {
        // 寻找满足条件的x坐标最小的
        if (it->get_x() > lb_conner.x && it->pt_bottle.y <= lb_conner.y
            && it->pt_top.y > lb_conner.y)
        {
            if (as_right_x > it->get_x() )
                as_right_x = it->get_x();
        }
    }

    // b2 寻找右上角的y坐标
    for (vector<Hline>::iterator it = g_v_hline.begin() ;  it != g_v_hline.end(); ++it)
    {
        if (it->get_y() > lb_conner.y && it->pt_right.x > lb_conner.x
            && it->pt_left.x < as_right_x)
        {
            if(as_top_y > it->get_y() )
                as_top_y = it->get_y();
        }
    }

    action_space as2(lb_conner,as_right_x - lb_conner.x, as_top_y - lb_conner.y);
    if (find(g_v_as.begin(), g_v_as.end(), as2) == g_v_as.end())
        g_v_as.push_back(as2);
}

// 左上角算法
void conner2as_lt(const conner & lt_conner)
{
    int as_bottle_y = MIN;
    int as_right_x = MAX;
    // a1 先向下扩展，寻找下边界,即动作空间的右下角的y坐标
    for (vector<Hline>::iterator it = g_v_hline.begin() ;  it != g_v_hline.end(); ++it)
    {
        // 寻找满足条件的y坐标的最大的
        if (it->get_y() < lt_conner.y && it->pt_left.x <= lt_conner.x
            && it->pt_right.x > lt_conner.x)
        {
            if (as_bottle_y < it->get_y() )
                as_bottle_y = it->get_y();
        }
    }

    // a2 寻找右边界，即右下角的x坐标
    for (vector<Vline>::iterator it = g_v_vline.begin() ; it != g_v_vline.end() ; it++)
    {
        // 寻找满足条件的x坐标最小的
        if (it->get_x() > lt_conner.x && it->pt_bottle.y < lt_conner.y
            && it->pt_top.y > as_bottle_y)
        {
            if (as_right_x > it->get_x() )
                as_right_x = it->get_x();
        }
    }

    action_space as(conner(lt_conner.x, as_bottle_y),as_right_x - lt_conner.x,
                    lt_conner.y - as_bottle_y );
    if (find(g_v_as.begin(), g_v_as.end(), as) == g_v_as.end())
        g_v_as.push_back(as);

    // b1 先向右扩展，寻找右边界，即动作空间的右下角的x坐标
    as_bottle_y = MIN;
    as_right_x = MAX;
    for (vector<Vline>::iterator it = g_v_vline.begin() ; it != g_v_vline.end() ; it++)
    {
        // 寻找满足条件的x坐标最小的
        if (it->get_x() > lt_conner.x && it->pt_top.y >= lt_conner.y
            && it->pt_bottle.y < lt_conner.y )
        {
            if (as_right_x > it->get_x() )
                as_right_x = it->get_x();
        }
    }

    // b2 下边界，寻找右下角的y坐标
    for (vector<Hline>::iterator it = g_v_hline.begin() ;  it != g_v_hline.end(); ++it)
    {
        // 寻找满足条件的y坐标的最大的
        if (it->get_y() < lt_conner.y && it->pt_right.x > lt_conner.x
            && it->pt_left.x < as_right_x)
        {
            if(as_bottle_y < it->get_y() )
                as_bottle_y = it->get_y();
        }
    }
    action_space as2(conner(lt_conner.x, as_bottle_y),as_right_x - lt_conner.x,
                    lt_conner.y - as_bottle_y );
    if (find(g_v_as.begin(), g_v_as.end(), as2) == g_v_as.end())
        g_v_as.push_back(as2);
}

// 右上角算法
void conner2as_rt(const conner & rt_conner)
{
    int as_bottle_y = MIN ;
    int as_left_x = MIN ;

    // a1 先向下扩展，寻找下边界，即左下角的y坐标
    for (vector<Hline>::iterator it = g_v_hline.begin() ;  it != g_v_hline.end(); ++it)
    {
        // 寻找满足条件的y坐标的最大的
        if (it->get_y() < rt_conner.y && it->pt_left.x < rt_conner.x
            && it->pt_right.x >= rt_conner.x)
        {
            if (as_bottle_y < it->get_y() )
                as_bottle_y = it->get_y();
        }
    }
    // a2 寻找左边界，即左下角的x坐标
    for (vector<Vline>::iterator it = g_v_vline.begin() ; it != g_v_vline.end() ; it++)
    {
        // 寻找满足条件的x坐标最大的
        if (it->get_x() < rt_conner.x && it->pt_bottle.y < rt_conner.y
            && it->pt_top.y > as_bottle_y)
        {
            if (as_left_x < it->get_x() )
                as_left_x = it->get_x();
        }
    }
    action_space as(conner(as_left_x, as_bottle_y),rt_conner.x - as_left_x,
                    rt_conner.y - as_bottle_y );
    if (find(g_v_as.begin(), g_v_as.end(), as) == g_v_as.end())
        g_v_as.push_back(as);

    // b1 先向左扩展，先寻找左边界，即左下角的x坐标
    as_bottle_y = MIN;
    as_left_x = MIN;
    for (vector<Vline>::iterator it = g_v_vline.begin() ; it != g_v_vline.end() ; it++)
    {
        // 寻找满足条件的x坐标最大的
        if (it->get_x() < rt_conner.x && it->pt_top.y >= rt_conner.y
            && it->pt_bottle.y < rt_conner.y )
        {
            if (as_left_x < it->get_x() )
                as_left_x = it->get_x();
        }
    }

    // b2 寻找下边界，即左下角的y坐标
    for (vector<Hline>::iterator it = g_v_hline.begin() ;  it != g_v_hline.end(); ++it)
    {
        // 寻找满足条件的y坐标的最大的
        if (it->get_y() < rt_conner.y && it->pt_right.x > as_left_x
            && it->pt_left.x < rt_conner.x )
        {
            if(as_bottle_y < it->get_y() )
                as_bottle_y = it->get_y();
        }
    }
    action_space as2(conner(as_left_x, as_bottle_y),rt_conner.x - as_left_x,
                    rt_conner.y - as_bottle_y );
    if (find(g_v_as.begin(), g_v_as.end(), as2) == g_v_as.end())
        g_v_as.push_back(as2);
}

// 右下角算法
void conner2as_rb(const conner & rb_conner)
{
    // a1 先向上扩展，寻找上界,即左上角的y坐标
    int as_top_y = MAX ;
    int as_left_x = MIN ;
    for (vector<Hline>::iterator it = g_v_hline.begin() ;  it != g_v_hline.end(); ++it)
    {
        // 寻找满足条件的y坐标的最小的
        if (it->get_y() > rb_conner.y && it->pt_right.x >= rb_conner.x
            && it->pt_left.x < rb_conner.x)
        {
            if(as_top_y > it->get_y() )
                as_top_y = it->get_y();
        }
    }

    // a2 寻找左边界，即左上角的x坐标
    for (vector<Vline>::iterator it = g_v_vline.begin() ; it != g_v_vline.end() ; it++)
    {
        // 寻找满足条件的x坐标最大的
        if (it->get_x() < rb_conner.x && it->pt_top.y > rb_conner.y
            && it->pt_bottle.y < as_top_y )
        {
            if(as_left_x < it->get_x() )
                as_left_x = it->get_x();
        }
    }
    action_space as( conner(as_left_x, rb_conner.y),rb_conner.x - as_left_x,
                     as_top_y - rb_conner.y);
    if (find(g_v_as.begin(), g_v_as.end(), as) == g_v_as.end())
        g_v_as.push_back(as);

    // b1 先向左扩展，寻找左上角的x坐标
    as_top_y = MAX ;
    as_left_x = MIN ;
    for (vector<Vline>::iterator it = g_v_vline.begin() ; it != g_v_vline.end() ; it++)
    {
        // 寻找满足条件的x坐标最大的
        if (it->get_x() < rb_conner.x && it->pt_bottle.y <= rb_conner.y
            && it->pt_top.y > rb_conner.y)
        {
            if (as_left_x < it->get_x() )
                as_left_x = it->get_x();
        }
    }
    // b2 寻找上边界，即左上角的y坐标
    for (vector<Hline>::iterator it = g_v_hline.begin() ;  it != g_v_hline.end(); ++it)
    {
        // 寻找满足条件的y坐标最小的
        if (it->get_y() > rb_conner.y && it->pt_right.x > as_left_x
            && it->pt_left.x < rb_conner.x )
        {
            if (as_top_y > it->get_y() )
                as_top_y = it->get_y();
        }
    }
    action_space as2( conner(as_left_x, rb_conner.y),rb_conner.x - as_left_x,
                     as_top_y - rb_conner.y);
    if (find(g_v_as.begin(), g_v_as.end(), as2) == g_v_as.end())
        g_v_as.push_back(as2);    
}



void find_conner_vline2downline(const Vline & vline, const Hline & down_line)
{
    if (vline.pt_top.y >= down_line.get_y() && vline.pt_bottle.y < down_line.get_y()
        && vline.get_x() >= down_line.pt_left.x && vline.get_x() <= down_line.pt_right.x)
    {
        // 如果垂直线是右沿线，那么和举行块下沿线组成角为左上角
        if (vline.line_type == RIGHT_LINE && vline.get_x()!= down_line.pt_right.x
            && g_s_conner_blocked.count(conner(vline.get_x(),down_line.get_y(),1,LEFT_TOP) )==0 )
        {
            g_s_conner.insert(conner(vline.get_x(),down_line.get_y(),1,LEFT_TOP) );
            g_s_conner2space.insert(conner(vline.get_x(),down_line.get_y(),1,LEFT_TOP));
        }
        
        // 左沿线，右上角
        if(vline.line_type == LEFT_LINE && vline.get_x()!= down_line.pt_left.x
            && g_s_conner_blocked.count(conner(vline.get_x(),down_line.get_y(),1,RIGHT_TOP) ) ==0) 
        {
            g_s_conner.insert(conner(vline.get_x(),down_line.get_y(),1,RIGHT_TOP) );
            g_s_conner2space.insert(conner(vline.get_x(),down_line.get_y(),1,RIGHT_TOP));
        }
        
    }
}

void find_conner_vline2upline(const Vline & vline, const Hline & up_line)
{
    if (vline.pt_bottle.y <= up_line.get_y() && vline.pt_top.y > up_line.get_y()
        && vline.get_x() >= up_line.pt_left.x && vline.get_x() <= up_line.pt_right.x )
    {
        // 垂直线是左沿线且不和矩形上沿线的左端点重合，右下角
        if (vline.line_type == LEFT_LINE && vline.get_x() != up_line.pt_left.x
            && g_s_conner_blocked.count(conner(vline.get_x(),up_line.get_y(),1,RIGHT_BOTTLE))==0 )
        {
            g_s_conner.insert(conner(vline.get_x(),up_line.get_y(),1,RIGHT_BOTTLE) );
            g_s_conner.insert(conner(vline.get_x(),up_line.get_y(),1,RIGHT_BOTTLE) );
        }
        
        // 垂直线是右沿线且不和矩形上沿线的右端点重合，左下角
        if (vline.line_type == RIGHT_LINE && vline.get_x() != up_line.pt_right.x
            && g_s_conner_blocked.count(conner(vline.get_x(),up_line.get_y(),1,LEFT_BOTTLE))==0)
        {
            g_s_conner.insert(conner(vline.get_x(),up_line.get_y(),1,LEFT_BOTTLE) );
            g_s_conner2space.insert(conner(vline.get_x(),up_line.get_y(),1,LEFT_BOTTLE) );
        }
        
    }
}

void find_conner_hline2leftline(const Hline & hline, const Vline & left_line)
{
    if (hline.pt_right.x >= left_line.get_x() && hline.pt_left.x < left_line.get_x()
        && hline.get_y() >= left_line.pt_bottle.y && hline.get_y() <= left_line.pt_top.y)
    {
        // 如果水平线是上沿线，则和矩形块的左沿线组成角为右下角
        if (hline.line_type == UP_LINE && hline.get_y() != left_line.pt_top.y
            && g_s_conner_blocked.count(conner(left_line.get_x(),hline.get_y(),1,RIGHT_BOTTLE))==0)
        {
            g_s_conner.insert(conner(left_line.get_x(),hline.get_y(),1,RIGHT_BOTTLE));
            g_s_conner2space.insert(conner(left_line.get_x(),hline.get_y(),1,RIGHT_BOTTLE));

        }
        // 如果水平线是下沿线，则和矩形块的左沿线组成角为右上角
        if (hline.line_type == DOWN_LINE && hline.get_y() != left_line.pt_bottle.y
            && g_s_conner_blocked.count(conner(left_line.get_x(),hline.get_y(),1,RIGHT_TOP))==0 )
        {
            g_s_conner.insert(conner(left_line.get_x(),hline.get_y(),1,RIGHT_TOP));
            g_s_conner2space.insert(conner(left_line.get_x(),hline.get_y(),1,RIGHT_TOP));
        }
        
    }
}

void find_conner_hline2rightline(const Hline & hline, const Vline & right_line)
{
    if (hline.pt_left.x <= right_line.get_x() && hline.pt_right.x > right_line.get_x()
        && hline.get_y() >= right_line.pt_bottle.y && hline.get_y() <= right_line.pt_top.y)
    {
        // 水平线是上沿线，且不和当前矩形右沿线的上端点重合，左下角
        if (hline.line_type == UP_LINE && hline.get_y() != right_line.pt_top.y
            && g_s_conner_blocked.count(conner(right_line.get_x(), hline.get_y(),1,LEFT_BOTTLE )) ==0)
        {
            g_s_conner.insert(conner(right_line.get_x(), hline.get_y(),1,LEFT_BOTTLE ));
            g_s_conner2space.insert(conner(right_line.get_x(), hline.get_y(),1,LEFT_BOTTLE ));
        }
        // 水平线是下沿线，且不和当前矩形右沿线的下端点重合，左上角
        if (hline.line_type == DOWN_LINE && hline.get_y() != right_line.pt_bottle.y
            && g_s_conner_blocked.count(conner(right_line.get_x(), hline.get_y(),1,LEFT_TOP )) == 0)
        {
            g_s_conner.insert(conner(right_line.get_x(), hline.get_y(),1,LEFT_TOP ));
            g_s_conner2space.insert(conner(right_line.get_x(), hline.get_y(),1,LEFT_TOP ));
        }
        
    }
}


void generate_conners(vector<rectangle>::iterator i2chonse_rec)
{
    Hline upl(i2chonse_rec->left_top(),i2chonse_rec->right_top(),UP_LINE);
    Hline downl(i2chonse_rec->left_bottle,i2chonse_rec->right_bottle(),DOWN_LINE);
    Vline leftl(i2chonse_rec->left_bottle, i2chonse_rec->left_top() ,LEFT_LINE);
    Vline rightl(i2chonse_rec->right_bottle(), i2chonse_rec->right_top() ,RIGHT_LINE);
    // 矩形块的4条线加入线集合中
    g_v_hline.push_back(upl);
    g_v_hline.push_back(downl);
    g_v_vline.push_back(leftl);
    g_v_vline.push_back(rightl);
    for (vector<Vline>::iterator it = g_v_vline.begin(); it != g_v_vline.end(); ++it)
    {
        find_conner_vline2upline(*it,upl);
        find_conner_vline2downline(*it,downl);
    }

    for (vector<Hline>::iterator it = g_v_hline.begin(); it != g_v_hline.end(); ++it)
    {
        find_conner_hline2leftline(*it,leftl);
        find_conner_hline2rightline(*it,rightl);
    }
    
}


// 寻找被矩形块 i2chonse_rec冲突的动作空间
// 并且把这几个动作空间的实角添加到 生成动作空间的角集合中
void find_conflict_as(vector<rectangle>::iterator i2chonse_rec)
{
    g_v_as_conflict.clear();
    g_s_conner2space.clear();

    
    rectangle_conflict rec_conflict(i2chonse_rec->left_bottle,i2chonse_rec->width,
                                    i2chonse_rec->height);
    for_each(g_v_as.begin(),g_v_as.end(),rec_conflict);
    set<conner>::iterator its = g_s_conner.end();
    for (vector<action_space>::iterator it = g_v_as.begin(); it != g_v_as.end() ; ++it)
    {
        // 如果此动作空间和小矩形 i2chonse_rec有交集
        if (it->is_conflict == 1)
        {
            // 动作空间加入冲突动作空间集合
            g_v_as_conflict.push_back(*it);
            // 动作空间的实角加入 生成动作空间的角集合中
            if ((its = g_s_conner.find(it->left_bottle) ) != g_s_conner.end())
                g_s_conner2space.insert(*its);
            if ((its = g_s_conner.find(it->left_top()) ) != g_s_conner.end())
                g_s_conner2space.insert(*its);
            if ((its = g_s_conner.find(it->right_top()) ) != g_s_conner.end())
                g_s_conner2space.insert(*its);
            if ((its = g_s_conner.find(it->right_bottle()) ) != g_s_conner.end())
                g_s_conner2space.insert(*its);
        }
        
    }
}

void update_action_space()
{
    // 由角生成动作空间
    for (set<conner>::iterator it = g_s_conner2space.begin();
         it != g_s_conner2space.end()  ; ++it)
    {
        switch( it->conner_type  )
        {
        case LEFT_BOTTLE: conner2as_lb(*it);break;
        case LEFT_TOP:    conner2as_lt(*it);break;
        case RIGHT_BOTTLE:conner2as_rb(*it);break;
        case RIGHT_TOP:   conner2as_rt(*it);break;
        }
    }

    // 清除和小矩形重叠的动作空间
    g_v_as.erase( remove_if(g_v_as.begin(),g_v_as.end(),is_conflicted),
                  g_v_as.end() );
    // for (vector<action_space>::iterator it = g_v_as_conflict.begin(); it != g_v_as_conflict.end(); it++)
    //     g_v_as.erase()
    //     g_v_as.erase(it);
}

// 输出最后数据
void output_data()
{
    ofstream ofile("output.txt");
    for (vector<rectangle>::iterator it = g_v_rec_done.begin(); it != g_v_rec_done.end(); it++)
        ofile<<it->width<<" "<<it->height<<"   ("<<it->left_bottle.x<<" , "<<it->left_bottle.y<<")"<<endl;
}


// test func
void print_data()
{
    cout<<"rec done:"<<endl;
    for (vector<rectangle>::iterator it = g_v_rec_done.begin(); it != g_v_rec_done.end(); it++)
        cout<<it->width<<" "<<it->height<<"   ("<<it->left_bottle.x<<" , "<<
            it->left_bottle.y<<")"<<" "<<it->reverse_mode<<endl;
    cout<<"as info:"<<endl;
    for (vector<action_space>::iterator it = g_v_as.begin(); it != g_v_as.end(); it++)
        cout<<"x: "<<it->left_bottle.x<<" y:"<<it->left_bottle.y<<
            " width:"<<it->width<<" height:"<<it->height<<endl;
    cout<<"conner:"<<endl;
    for (set<conner>::iterator it = g_s_conner.begin(); it != g_s_conner.end() ; ++it)
    {
        cout<<"x:"<<it->x<<" y:"<<it->y<<" conner type:";
        switch(it->conner_type)
        {
        case LEFT_BOTTLE:cout<<" lb ";            break;
        case LEFT_TOP:cout<<" lt";            break;
        case RIGHT_TOP:cout<<" rt";
            break;
        case RIGHT_BOTTLE:cout<<" rb";
            break;
        }
        cout<<endl;
    }
    
}

bool is_conflicted(const action_space& as)
{
    if (as.is_conflict)
        return 1;
    return 0;
}
