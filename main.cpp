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
#include <stack>
#include <ctime>

using namespace std;

void print_kopt();


stack< vector<Hline>  > g_stk_v4hl;
stack< vector<Vline> > g_stk_v4vl;
stack< vector<rectangle> > g_stk_v4rec_undo;
stack< vector<rectangle> > g_stk_v4rec_done;
stack< vector<action_space> > g_stk_v4as;
stack< set<conner> > g_stk_s4conner;
stack< vector<conner_action> > g_stk_v4kopt;

// 前k个最优的占角动作
vector<conner_action> g_v_action_kopt;
int  g_optnumber = 3;

vector<Hline> g_v_hline; // 所有水平线
vector<Vline> g_v_vline ; // 所有垂直线


vector<rectangle> g_v_rec_undo ; // 未处理的小矩形
vector<rectangle> g_v_rec_done ; // 已经处理的小矩形
vector<rectangle> g_v_rec_scheduled;//时间调度已经加工完的小举行块
vector<rectangle> g_v_rec_last_unfinished; // 保存上次调度未完成的举行块

vector<action_space> g_v_as ; // 动作空间
vector<action_space> g_v_as_conflict ;// 保存被放入矩形影响的动作空间

set<conner> g_s_conner; // 所有的实角

set<conner> g_s_conner_blocked; // 记录当前被屏蔽的角
set<conner> g_s_conner2space; // 每次新生成的角,新动作空间从这些角产生

// 

action_space g_as(point(0,0),0,0);
ofstream ofile("output.txt");

const int MAX = 999999;
const int MIN = -99999;

void print_data();
void print_status_task();



// 选择当前最优的放置
bool chose_as_rec();

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
bool  max_fd_of4values(const vector<rectangle>:: iterator & i2rec,
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
void generate_conners(const rectangle& rec);

// 寻找被矩形块 i2chonse_rec冲突的动作空间
// 并且把这几个动作空间的实角添加到 生成动作空间的角集合中
void find_conflict_as(const rectangle & rec);

//
bool is_conflicted(const action_space& as);

// 更新动作空间
void update_action_space();

    
void update_kopt( const fit_degree & fd,
                  const rectangle & rec,
                  const action_space & as);

int get_area();

// 初始化动作空间和线信息
void init_data();


// 选择最优的小木块和动作空间后，完成一次占角，更新数据
void update_data(vector<rectangle>::iterator  i2chonse_rec,
                 vector<action_space>::iterator i2chonse_as);


void task_scheduling();

int update_rec_status();


void print_schedule(int time,int number);


// 初始化操作
void init();

// 输出最后数据
void output_data(int number, int time);

void print_kopt();

void data_push();

void data_pop();

int backtrack2();

bool conner_check(const conner & cn,int conner_type);


//在待加工矩形块集合中，选择加工时间最长的和 rec相等的举行块
void chonse_biggest_time_rec(vector<rectangle>::iterator &i2rec,const rectangle & rec);


bool rec_equal_test_withtime(const rectangle &rec1, const rectangle &rec2);

void vline_insert(const Vline & vl );
void hline_insert(const Hline & hl);


int main(int arg ,char *arv[])
{
    clock_t start = clock();
    init();
    task_scheduling();
    clock_t end = clock();
    double duration =(double)(end-start)/CLOCKS_PER_SEC;
    cout<<duration<<endl;
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
        ifile >> rec.time ;
        rec.id = i ;
        g_v_rec_undo.push_back(rec);
    }
    // 预分配空间
    // 否则会导致 chose_as_rec 迭代时候，迭代器失效，因为
    g_v_as.reserve(200);
    // 预分配空间,放置迭代起失效
    g_v_vline.reserve(1000);
    g_v_hline.reserve(1000);
    g_v_action_kopt.reserve(50);
    g_v_rec_undo.reserve(100);
    g_v_rec_done.reserve(100);

    init_data();
}

// 消除被覆盖的实角
void remove_conner_blocked(rectangle & rec)
{
    g_s_conner_blocked.clear();
    set<conner>::iterator it = g_s_conner.end();
    // 被屏蔽的角保存下来，生成角的时候用来再次屏蔽
    if ((it = g_s_conner.find(conner(rec.left_top()) ))!= g_s_conner.end() )
    {
        g_s_conner_blocked.insert(*it);
        g_s_conner.erase(it);
    }
    if ((it = g_s_conner.find(conner(rec.right_top())) )!= g_s_conner.end() )
    {
        g_s_conner_blocked.insert(*it);
        g_s_conner.erase(it);
    }
    if ((it = g_s_conner.find(conner(rec.left_bottle)) )!= g_s_conner.end() )
    {
        g_s_conner_blocked.insert(*it);
        g_s_conner.erase(it);
    }
    if ((it = g_s_conner.find(conner(rec.right_bottle())) )!= g_s_conner.end() )
    {
        g_s_conner_blocked.insert(*it);
        g_s_conner.erase(it);
    }

    g_s_conner2space.erase(conner(rec.left_top()));
    g_s_conner2space.erase(conner(rec.right_top()));
    g_s_conner2space.erase(conner(rec.left_bottle));
    g_s_conner2space.erase(conner(rec.right_bottle()));
}

// 选择动作空间和小木块
bool chose_as_rec()
{
    int backtrack_mark = 1;
    if (g_v_rec_undo.size() == 0)
        return false;
    vector<rectangle>::iterator i2rec = g_v_rec_undo.begin();
    vector<action_space>::iterator i2as = g_v_as.begin();
    fit_degree fd;
    bool finded = 0;
    
    for (i2as= g_v_as.begin(); i2as!= g_v_as.end() ; i2as++) // 迭代动作空间
    {
        for (i2rec=g_v_rec_undo.begin(); i2rec != g_v_rec_undo.end() ; ++i2rec) // 迭代外部矩形块
        {
            if(!max_fd_of8values(i2rec,i2as,fd))
                continue;
            finded = 1;;
            // 
            if (backtrack_mark == 1)
            {
                g_v_action_kopt.clear();
                backtrack_mark = 0;
            }
            update_kopt(fd,*i2rec,*i2as) ;
        }
    }
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
//
int calculate_fd_s(vector<rectangle>::iterator i2rec,
                      vector<action_space>::iterator i2as)
{
    rectangle rec_chonse;
    action_space as_chonse;
    // backup
    data_push();
    
    rec_chonse = *i2rec;
    as_chonse = *i2as;
    find_conflict_as(*i2rec);
    remove_conner_blocked(rec_chonse);
    generate_conners(*i2rec);
    update_action_space();
//    g_v_rec_done.push_back(*i2rec);
//    g_v_rec_undo.erase(i2rec);
    int s = g_v_as.size();
    // restore
    data_pop();
    return 0-s;
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
bool  max_fd_of4values(const vector<rectangle>:: iterator & i2rec,
            const vector<action_space>::iterator & i2as,
            fit_degree & fd)
{
    // 放不下，则直接取为-1
    if (i2rec->width > i2as->width || i2rec->height> i2as->height)
    {
        fd.k = -1 ;
        return 0;
    }
    
    fit_degree fd_max ; // 本动作空间针对当前木块目前最大的fd
    rectangle rec_op = *i2rec; // 当前最优的小木块

    // left bottle 是实角
    if(g_s_conner.count(conner(i2as->left_bottle)))
    {
        i2rec->set_ordinate_lb(i2as->left_bottle);
        calculate_fd(i2rec,i2as,fd_max);
        i2as->place_type = LEFT_BOTTLE ;
        rec_op = *i2rec;
    }

    // left top 是实角
    if (g_s_conner.count(conner(i2as->left_top())))
    {
        i2rec->set_ordinate_lt(i2as->left_top() );
        calculate_fd(i2rec,i2as,fd);
        if (fd_max < fd || (fd_max == fd &&   *i2rec > rec_op) )
        {
            fd_max = fd ;
            rec_op = *i2rec;
            i2as->place_type = LEFT_TOP ;
        }   
    }
        
    // right bottle 是实角
    if (g_s_conner.count(conner(i2as->right_bottle())))
    {
        i2rec->set_ordinate_rb(i2as->right_bottle() );
        calculate_fd(i2rec,i2as,fd);
        if (fd_max < fd || (fd_max == fd &&  *i2rec > rec_op ) )
        {
            fd_max = fd ;
            rec_op = *i2rec;
            i2as->place_type = RIGHT_BOTTLE ;
        }
    }

    // right top 是 实角
    if (g_s_conner.count(conner(i2as->right_top())))
    {
        i2rec->set_ordinate_rt(i2as->right_top() );
        calculate_fd(i2rec,i2as,fd);
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
        cout<<"rec:"<<i2rec->width<<" "<<i2rec->height;
        cout<<"  (:"<<i2rec->left_bottle.x<<" "<<i2rec->left_bottle.y<<" )";
        cout<<"  as:"<<i2as->width<<" "<<i2as->height;
        cout<<"  (:"<<i2as->left_bottle.x<<" "<<i2as->left_bottle.y<<" )"<<endl;

    }
    return 1;
}

// 计算小矩形在动作空间 i2as 四个角的fd，包括水平和垂直方向
// 也即有4*2 个值选取
bool max_fd_of8values(const vector<rectangle>:: iterator & i2rec,
                      const vector<action_space>::iterator & i2as,
            fit_degree & fd)
{
    // 放不下,则直接返回false
    if ( min(i2rec->width,i2rec->height) > min(i2as->width,i2as->height) 
         ||max(i2rec->width,i2rec->height)>  max(i2as->width,i2as->height) )
    {
        return 0;
    }
    bool finded = 0;
    finded = max_fd_of4values(i2rec , i2as, fd);
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

    return 1 ;
    
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
            if (conner_check(conner(vline.get_x(),down_line.get_y(),1,LEFT_TOP) ,LEFT_TOP))
            {
                g_s_conner.insert(conner(vline.get_x(),down_line.get_y(),vline,down_line,1,LEFT_TOP) );
                g_s_conner2space.insert(conner(vline.get_x(),down_line.get_y(),vline,down_line,1,LEFT_TOP));
            }
        }
        
        // 左沿线，右上角
        if(vline.line_type == LEFT_LINE && vline.get_x()!= down_line.pt_left.x
            && g_s_conner_blocked.count(conner(vline.get_x(),down_line.get_y(),1,RIGHT_TOP) ) ==0) 
        {
            if (conner_check(conner(vline.get_x(),down_line.get_y(),1,RIGHT_TOP) , RIGHT_TOP) )
            {
                g_s_conner.insert(conner(vline.get_x(),down_line.get_y(),vline,down_line,1,RIGHT_TOP) );
                g_s_conner2space.insert(conner(vline.get_x(),down_line.get_y(),vline,down_line,1,RIGHT_TOP));
            }

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
            if (conner_check(conner(vline.get_x(),up_line.get_y(),1,RIGHT_BOTTLE) ,RIGHT_BOTTLE))
            {
                g_s_conner.insert(conner(vline.get_x(),up_line.get_y(),vline,up_line,1,RIGHT_BOTTLE) );
                g_s_conner2space.insert(conner(vline.get_x(),up_line.get_y(),vline,up_line,1,RIGHT_BOTTLE) );
            }

        }
        
        // 垂直线是右沿线且不和矩形上沿线的右端点重合，左下角
        if (vline.line_type == RIGHT_LINE && vline.get_x() != up_line.pt_right.x
            && g_s_conner_blocked.count(conner(vline.get_x(),up_line.get_y(),1,LEFT_BOTTLE))==0)
        {
            if (conner_check(conner(vline.get_x(),up_line.get_y(),1,LEFT_BOTTLE),LEFT_BOTTLE))
            {
                g_s_conner.insert(conner(vline.get_x(),up_line.get_y(),vline,up_line,1,LEFT_BOTTLE) );
                g_s_conner2space.insert(conner(vline.get_x(),up_line.get_y(),vline,up_line,1,LEFT_BOTTLE) );
            }
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
            if (conner_check(conner(left_line.get_x(),hline.get_y(),1,RIGHT_BOTTLE),RIGHT_BOTTLE))
            {
                g_s_conner.insert(conner(left_line.get_x(),hline.get_y(),left_line,hline,1,RIGHT_BOTTLE));
                g_s_conner2space.insert(conner(left_line.get_x(),hline.get_y(),left_line,hline,1,RIGHT_BOTTLE));
            }
        }
        // 如果水平线是下沿线，则和矩形块的左沿线组成角为右上角
        if (hline.line_type == DOWN_LINE && hline.get_y() != left_line.pt_bottle.y
            && g_s_conner_blocked.count(conner(left_line.get_x(),hline.get_y(),1,RIGHT_TOP))==0 )
        {
            if (conner_check(conner(left_line.get_x(),hline.get_y(),1,RIGHT_TOP),RIGHT_TOP))
            {
                g_s_conner.insert(conner(left_line.get_x(),hline.get_y(),left_line,hline,1,RIGHT_TOP));
                g_s_conner2space.insert(conner(left_line.get_x(),hline.get_y(),left_line,hline,1,RIGHT_TOP));
            }
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
            if (conner_check(conner(right_line.get_x(), hline.get_y(),1,LEFT_BOTTLE ),LEFT_BOTTLE))
            {
                g_s_conner.insert(
                    conner(right_line.get_x(), hline.get_y(),right_line,hline,1,LEFT_BOTTLE ));
                g_s_conner2space.insert(
                    conner(right_line.get_x(), hline.get_y(),right_line,hline,1,LEFT_BOTTLE ));
            }

        }
        // 水平线是下沿线，且不和当前矩形右沿线的下端点重合，左上角
        if (hline.line_type == DOWN_LINE && hline.get_y() != right_line.pt_bottle.y
            && g_s_conner_blocked.count(conner(right_line.get_x(), hline.get_y(),1,LEFT_TOP )) == 0)
        {
            if (conner_check(conner(right_line.get_x(), hline.get_y(),1,LEFT_TOP ), LEFT_TOP))
            {
                g_s_conner.insert(
                    conner(right_line.get_x(), hline.get_y(),right_line,hline,1,LEFT_TOP ));
                g_s_conner2space.insert(
                    conner(right_line.get_x(), hline.get_y(),right_line,hline,1,LEFT_TOP ));
            }
        }
    }
}


void generate_conners(const rectangle & rec)
{
    Hline upl(rec.left_top(),rec.right_top(),UP_LINE);
    Hline downl(rec.left_bottle,rec.right_bottle(),DOWN_LINE);
    Vline leftl(rec.left_bottle, rec.left_top() ,LEFT_LINE);
    Vline rightl(rec.right_bottle(), rec.right_top() ,RIGHT_LINE);
    // 矩形块的4条线加入线集合中
    vline_insert(leftl);
    vline_insert(rightl);
    hline_insert(upl);
    hline_insert(downl);
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
void find_conflict_as(const rectangle & rec)
{
    g_v_as_conflict.clear();
    g_s_conner2space.clear();
    rectangle_conflict rec_conflict(rec.left_bottle,rec.width,
                                    rec.height);
    for_each(g_v_as.begin(),g_v_as.end(),rec_conflict);
    set<conner>::iterator its = g_s_conner.end();
    for (vector<action_space>::iterator it = g_v_as.begin(); it != g_v_as.end() ; ++it)
    {
        // 如果此动作空间和小矩形 rec有交集
        if (it->is_conflict == 1)
        {
            // 动作空间加入冲突动作空间集合
            g_v_as_conflict.push_back(*it);
            // 动作空间的实角加入 生成动作空间的角集合中
            if ((its = g_s_conner.find(conner(it->left_bottle)) ) != g_s_conner.end())
                g_s_conner2space.insert(*its);
            if ((its = g_s_conner.find(conner(it->left_top())) ) != g_s_conner.end())
                g_s_conner2space.insert(*its);
            if ((its = g_s_conner.find(conner(it->right_top())) ) != g_s_conner.end())
                g_s_conner2space.insert(*its);
            if ((its = g_s_conner.find(conner(it->right_bottle())) ) != g_s_conner.end())
                g_s_conner2space.insert(*its);
        }
    }
}

void update_action_space()
{
    // 由角生成动作空间
    for (set<conner>::iterator it = g_s_conner2space.begin();
         it != g_s_conner2space.end()   ; ++it)
    {
        switch( it->conner_type  )
        {
        case LEFT_BOTTLE: conner2as_lb(*it);break;
        case LEFT_TOP:    conner2as_lt(*it);break;
        case RIGHT_BOTTLE:conner2as_rb(*it);break;
        case RIGHT_TOP:   conner2as_rt(*it);break;
        }
    }
    // 去除受和放入矩形块冲突的动作空间
    g_v_as.erase( remove_if(g_v_as.begin(),g_v_as.end(),is_conflicted),
                  g_v_as.end() );
}

// 输出最后数据
void output_data(int number, int time)
{
    int area = 0;
    ofile<<"P"<<number<<"  time:"<<time;
    for (vector<rectangle>::iterator it = g_v_rec_done.begin(); it != g_v_rec_done.end(); it++)
        area+= it->get_area();
    ofile<<"  area:"<<area<<endl;
    for (vector<rectangle>::iterator it = g_v_rec_done.begin(); it != g_v_rec_done.end(); it++)
        ofile<<"R"<<it->id<<"   "<<it->width<<"   "<<it->height<<"     ("<<it->left_bottle.x<<" , "<<
            it->left_bottle.y<<")"<<"    "<<it->reverse_mode<<endl;
}



bool is_conflicted(const action_space& as)
{
    if(as.is_conflict == 0)
        return 0;
    return 1;
}


void update_kopt( const fit_degree & fd,
                  const rectangle & rec,
                  const action_space & as)
{
    vector<conner_action>::iterator it = g_v_action_kopt.end();

    // 如果已经存在一样的占角，则不添加
    it = find(g_v_action_kopt.begin(),g_v_action_kopt.end(),
              conner_action(fd,rec,as)) ;
    if (it != g_v_action_kopt.end())
        return ;
    
    if (g_v_action_kopt.size() <= g_optnumber)
        g_v_action_kopt.push_back(conner_action(fd,rec,as));
    else
    {
        it = min_element(g_v_action_kopt.begin(),g_v_action_kopt.end());
        if (*it < conner_action(fd,rec,as))
            *it = conner_action(fd,rec,as);
    }
}

int get_area()
{
    int area=0;
    for (vector<rectangle>::iterator it = g_v_rec_done.begin() ;
         it != g_v_rec_done.end() ; ++it)
        area += it->get_area();
    return area;
}


// 选择最优的小木块和动作空间后，完成一次占角，更新数据
void update_data(vector<rectangle>::iterator  i2chonse_rec,
                 vector<action_space>::iterator i2chonse_as)
{
    find_conflict_as(*i2chonse_rec);
    remove_conner_blocked(*i2chonse_rec);
    generate_conners(*i2chonse_rec);
    update_action_space();
    g_v_rec_done.push_back(*i2chonse_rec);
    g_v_rec_undo.erase(i2chonse_rec);
}


void task_scheduling()
{
    vector<rectangle>::iterator i2chonse_rec = g_v_rec_undo.begin();
    vector<action_space>::iterator i2chonse_as = g_v_as.begin();
    rectangle rec_chonse ;
    action_space as_chonse;
    int time_total = 0; // 记录总体加工时间
    int time_this = 0; // 记录本次调度加工时间
    int number = 0;
    while(g_v_rec_undo.size()!=0)
    {
        // 用回溯法完成一次调度
        backtrack2();
        time_this = update_rec_status();
        number++;
        time_total += time_this;
        print_schedule(time_total,number);
        output_data(number,time_total);
        cout<<g_v_rec_done.size()<<endl;
        
        init_data();
     }
}


// 初始化动作空间和线信息
void init_data()
{
    // 清空栈
    while(!g_stk_v4hl.empty())
        g_stk_v4hl.pop();
    while(!g_stk_v4vl.empty())
        g_stk_v4vl.pop();
    while(!g_stk_v4rec_undo.empty())
        g_stk_v4rec_undo.pop();
    while(!g_stk_v4rec_done.empty())
        g_stk_v4rec_done.pop();
    while(!g_stk_s4conner.empty())
        g_stk_s4conner.pop();
    while(!g_stk_v4kopt.empty())
        g_stk_v4kopt.pop();
    
    g_v_as.clear();
    g_v_as_conflict.clear();
    g_v_action_kopt.clear();
    g_v_rec_done.clear();
    g_s_conner_blocked.clear();
    g_s_conner2space.clear();

    g_v_as.push_back(g_as);
    
    for (vector<rectangle>::iterator it = g_v_rec_undo.begin();
         it!= g_v_rec_undo.end(); ++it)
        it->left_bottle.x = it->left_bottle.y = 0;
    
    // 初始化角
    g_s_conner.clear();
    Hline up_line(g_as.left_top(),g_as.right_top(),DOWN_LINE);
    Hline down_line(g_as.left_bottle,g_as.right_bottle(),UP_LINE);
    Vline left_line(g_as.left_bottle,g_as.left_top(),RIGHT_LINE);
    Vline right_line(g_as.right_bottle(),g_as.right_top(), LEFT_LINE);
    

    g_s_conner.insert(
        conner(g_as.left_bottle.x,g_as.left_bottle.y,left_line,down_line,1,LEFT_BOTTLE) );
    g_s_conner.insert(
        conner(g_as.left_top().x,g_as.left_top().y,left_line,up_line,1,LEFT_TOP ));
    g_s_conner.insert(
        conner(g_as.right_top().x,g_as.right_top().y,right_line,up_line,1,RIGHT_TOP ));
    g_s_conner.insert(
        conner(g_as.right_bottle().x,g_as.right_bottle().y,right_line,down_line,1,RIGHT_BOTTLE ));

    // 初始化线
    g_v_hline.clear();
    g_v_vline.clear();
    hline_insert(down_line);
    hline_insert(up_line);
    vline_insert(left_line);
    vline_insert(right_line);
}

bool time_comp(const rectangle & rec1,const rectangle & rec2)
{
    if (rec1.time < rec2.time)
        return 1;
    else
        return 0;
}

bool is_done(const rectangle & rec)
{
    if (rec.time==0)
        return 1;
    else
        return 0;
}

// 更新举行块状态，时间，以及所属集合
// 并且返回这次调度的举行块的加工时间，即加工时间最短的那个矩形块
int update_rec_status()
{

    g_v_rec_last_unfinished.clear();
    vector<rectangle>::iterator it = g_v_rec_done.end();
    
    it = min_element(g_v_rec_done.begin(),g_v_rec_done.end(),time_comp);
    int min_time = 0;
    if (it == g_v_rec_done.end())
        return -1 ;
    min_time = it->time;
    for(vector<rectangle>::iterator itr = g_v_rec_done.begin();
        itr != g_v_rec_done.end(); itr++)
    {
        itr->time  -= min_time;
        if (itr->time == 0) // 加工完毕的话，添加到调度完成集合
            g_v_rec_scheduled.push_back(*itr);
        if (itr->time != 0) // 加工未完成，重新添加到待处理集合
        {
            g_v_rec_undo.push_back(*itr);
            g_v_rec_last_unfinished.push_back(*itr);
        }
    }
    return min_time;
}


void print_schedule(int time,int number)
{
    int area = 0;
    for (vector<rectangle>::iterator it = g_v_rec_done.begin(); it != g_v_rec_done.end(); it++)
        area+= it->get_area();
    cout<<"P"<<number<<"  time:"<<time<<"  area:"<<area<<endl;
    for (vector<rectangle>::iterator it = g_v_rec_done.begin(); it != g_v_rec_done.end(); it++)
        cout<<"R"<<it->id<<"  "<<it->width<<"   "<<it->height<<"     ("<<it->left_bottle.x<<" , "<<
            it->left_bottle.y<<")"<<"    "<<it->reverse_mode<<endl;
    cout<<"end once task"<<endl;
}

void print_data()
{
    cout<<"-----------------begin-------------------------------"<<endl;
    cout<<"done info:"<<endl;
    for (vector<rectangle>::iterator it = g_v_rec_done.begin(); it != g_v_rec_done.end(); it++)
        cout<<it->width<<"   "<<it->height<<"     ("<<it->left_bottle.x<<" , "<<
            it->left_bottle.y<<")"<<"    "<<it->reverse_mode<<endl;
    
    cout<<" undo info:"<<endl;
    for (vector<rectangle>::iterator it = g_v_rec_undo.begin(); it != g_v_rec_undo.end(); it++)
        cout<<it->width<<"   "<<it->height<<"     ("<<it->left_bottle.x<<" , "<<
            it->left_bottle.y<<")"<<"    "<<it->reverse_mode<<endl;
    cout<<" as info size:"<<g_v_as.size()<<endl;
    for (vector<action_space>::iterator it = g_v_as.begin(); it != g_v_as.end(); it++)
        cout<<it->width<<"   "<<it->height<<"     ("<<it->left_bottle.x<<" , "<<
            it->left_bottle.y<<")"<<"    "<<it->reverse_mode<<endl;
    cout<<"conner info :"<<endl;
    for (set<conner>::iterator it = g_s_conner.begin(); it != g_s_conner.end(); it++)
    {
        cout<<"("<<it->pt.x<<", "<<it->pt.y<<")  ";
        cout<<"hl: left("<<it->hl.pt_left.x<<","<<it->hl.pt_left.y<<") ";
        cout<<"right("<<it->hl.pt_right.x<<","<<it->hl.pt_right.y<<");  ";
        cout<<"vl: bottle("<<it->vl.pt_bottle.x<<","<<it->vl.pt_bottle.y<<") ";
        cout<<"top("<<it->vl.pt_top.x<<","<<it->vl.pt_top.y<<")";
        cout<<endl;
    }
    

    cout<<"conner2space info :"<<endl;
    for (set<conner>::iterator it = g_s_conner2space.begin(); it != g_s_conner2space.end(); it++)
        cout<<"("<<it->pt.x<<", "<<it->pt.y<<")"<<endl;
    cout<<"-----------------end---------------------------------"<<endl;
    
}

void print_kopt()
{
    vector<conner_action>::iterator it = g_v_action_kopt.end();

    // 如果已经存在一样的占角，则不添加
    for (it =g_v_action_kopt.begin();
         it != g_v_action_kopt.end();it++)
    {
        cout<<"rec:"<<it->rec.width<<"  "<< it->rec.height<<"  ("<<it->rec.left_bottle.x;
        cout<<" "<<it->rec.left_bottle.y<<" )";
        cout<<"as:"<<it->as.width<<"  "<< it->as.height<<"  ("<<it->as.left_bottle.x;
        cout<<" "<<it->as.left_bottle.y<<" )"<<endl;
    }
}


int backtrack2()
{
    vector<rectangle>::iterator i2chonse_rec = g_v_rec_undo.begin();
    vector<action_space>::iterator i2chonse_as = g_v_as.begin();
    int area = 0;
    int max_area = 0;
    fit_degree fd;
    rectangle rec;
    action_space as;
    conner_action ac(fd,rec,as);
    
    if(chose_as_rec())
    {
        // 反向排序，这样g_v_action中数据按从大到小排序
        sort(g_v_action_kopt.rbegin(),g_v_action_kopt.rend());
        max_area = 0;
        for (vector<conner_action>::iterator
                 it = g_v_action_kopt.begin();
             it != g_v_action_kopt.end() ; ++it)
        {
            chonse_biggest_time_rec(i2chonse_rec,it->rec);
            
            if(i2chonse_rec == g_v_rec_undo.end())
                cout<<"not find"<<endl;
            *i2chonse_rec = it->rec;
            
            i2chonse_as = find(g_v_as.begin(),g_v_as.end(),it->as);
            data_push();
            update_data(i2chonse_rec,i2chonse_as);
            area = backtrack2();
            if(area == g_as.get_area() || g_v_rec_undo.size()==0)
                return area;
            data_pop();
            if(max_area < area)
            {
                max_area = area;
                ac = *it;
            }
        }
    }
    area = get_area();
    return area;
}

void data_push()
{
    g_stk_v4hl.push( g_v_hline );
    g_stk_v4vl.push( g_v_vline);
    g_stk_v4rec_undo.push( g_v_rec_undo);
    g_stk_v4rec_done.push( g_v_rec_done);
    g_stk_v4as.push(g_v_as);
    g_stk_s4conner.push( g_s_conner);
    g_stk_v4kopt.push( g_v_action_kopt);
}

void data_pop()
{
    g_v_hline = g_stk_v4hl.top();
    g_stk_v4hl.pop();

    g_v_vline = g_stk_v4vl.top();
    g_stk_v4vl.pop();

    g_v_rec_undo = g_stk_v4rec_undo.top();
    g_stk_v4rec_undo.pop();

    g_v_rec_done = g_stk_v4rec_done.top();
    g_stk_v4rec_done.pop();

    g_v_as = g_stk_v4as.top();
    g_stk_v4as.pop();

    g_s_conner = g_stk_s4conner.top();
    g_stk_s4conner.pop();

    g_v_action_kopt = g_stk_v4kopt.top();
    g_stk_v4kopt.pop();
    
}

bool conner_check(const conner & cn,int conner_type)
{
    for (vector<rectangle>::iterator it = g_v_rec_done.begin();
         it!= g_v_rec_done.end() ; ++it)
    {
        switch(conner_type)
        {
        case LEFT_TOP:
            if(it->left_top() == cn.pt)
                return 0;
            break;
        case LEFT_BOTTLE:
            if(it->left_bottle == cn.pt)
                return 0;
            break;
        case RIGHT_TOP:
            if (it->right_top() == cn.pt)
                return 0;
            break;
        case RIGHT_BOTTLE:
            if (it->right_bottle() == cn.pt)
                return 0;
            break;
        default:cout<<"error conner check"<<endl;
        }
    }
    return 1;
}


// 在同等条件下，选择加工时间最长的那个举行块，
// 即在矩形宽高相等的情况下，选择加工时间最长的那个
void chonse_biggest_time_rec(vector<rectangle>::iterator &i2rec,const rectangle & rec)
{
    int time = 0;
    i2rec = g_v_rec_undo.end();
    for (vector<rectangle>::iterator it = g_v_rec_undo.begin(); it != g_v_rec_undo.end(); it++)
    {
        if (*it == rec && it->time > time)
        {
            i2rec = it;
            time = it->time;
        }
    }
}

bool rec_equal_test_withtime(const rectangle &rec1, const rectangle &rec2)
{
    if (rec1 == rec2 && rec1.time == rec2.time)
        return 1;
    return 0;
}

// 测试本次调度的矩形块集合中，是否包含上次未完成的举行块
bool is_schedule_valid()
{
    // 排序
    sort(g_v_rec_done.begin(),g_v_rec_done.end());
    sort(g_v_rec_last_unfinished.begin(),g_v_rec_last_unfinished.end());
    // 求是否为子集
    if (includes(g_v_rec_done.begin(),g_v_rec_done.end(),
                 g_v_rec_last_unfinished.begin(),g_v_rec_last_unfinished.end(),
                 rec_equal_test_withtime) )
        return 1;
    return 0;
}

//左上角更新算法
void conner2as_lt(const conner & lt_conner)
{
    Vline vl_max;
    // 第一条在lt_conner右边的垂直线，从左到右的顺序
    vector<Vline>::iterator ivl_begin = upper_bound(g_v_vline.begin(),g_v_vline.end(),lt_conner.vl);
    // 第一条在lt_conner下面的水平线,从上到下的顺序
    vector<Hline>::iterator ihl_end = lower_bound(g_v_hline.begin(),g_v_hline.end(),lt_conner.hl);
    if (ihl_end == g_v_hline.begin())
        return ;
    else
        ihl_end--;
    
    for (vector<Vline>::iterator it = ivl_begin; it!=g_v_vline.end(); it++)
    {
        //保证左沿线
        if (it->line_type == RIGHT_LINE)
            continue;
        if (it->is_higher(vl_max))
            vl_max = *it;
        else
            continue;

        // 右边界
        if ( it->line_type==LEFT_LINE &&  it->pt_bottle.y < lt_conner.pt.y )
        {
            for (vector<Hline>::iterator ith = ihl_end ;
                 ith != --g_v_hline.begin(); ith--)
            {
                //保证上沿线
                if (ith->line_type == DOWN_LINE)
                    continue;
                // 水平线在垂直线上部且左端点在垂直线左边,则屏蔽此垂直线
                if (ith->get_y() >= it->pt_top.y && ith->pt_left.x < it->get_x() )
                    break;
                // 满足下边界
                if (ith->get_y() < it->pt_top.y && ith->pt_left.x < it->get_x())
                {
                    action_space as(point(lt_conner.pt.x, ith->get_y() ),
                                    it->get_x() -lt_conner.pt.x  , lt_conner.pt.y - ith->get_y());
                    if (find(g_v_as.begin(), g_v_as.end(), as) == g_v_as.end())
                        g_v_as.push_back(as);
                    break;
                }
            }
        }
        // 如果当前最高的直线上端点y坐标大于lt_conner的y坐标，则屏蔽后面所有直线
        if (vl_max.pt_top.y >= lt_conner.pt.y)
            break;
    }
}

// 左下角更新算法
void conner2as_lb(const conner & lb_conner)
{
    Vline vl_min(point(g_as.right_top()),point(g_as.right_top()),0);

    // 第一条在lb_conner右边的垂直线，从左到右的顺序
    vector<Vline>::iterator ivl_begin = upper_bound(g_v_vline.begin(),
                                                    g_v_vline.end(),lb_conner.vl);
    // 第一条在lb_conner上面的水平线,从上到下的顺序
    vector<Hline>::iterator ihl_begin = upper_bound(g_v_hline.begin(),
                                                  g_v_hline.end(),lb_conner.hl);
    for (vector<Vline>::iterator itv = ivl_begin; itv!= g_v_vline.end(); ++itv)
    {
        //保证左沿线
        if (itv->line_type == RIGHT_LINE)
            continue;
        if (itv->is_lower(vl_min))
            vl_min = *itv;
        else
            continue;
        // 右边界
        if (itv->line_type == LEFT_LINE && itv->pt_top.y > lb_conner.pt.y)
        {
            // 上边界
            for (vector<Hline>::iterator ith = ihl_begin;
                 ith != g_v_hline.end() ; ++ith)
            {
                // 保证必须是下沿线
                if (ith->line_type == UP_LINE)
                    continue;
                // 水平线在垂直线下部且左端点在垂直线左边，则屏蔽此垂直线
                if (ith->get_y() <= itv->pt_bottle.y && ith->pt_left.x < itv->get_x())
                    break;
                // 满足上边界
                if (ith->get_y() > itv->pt_bottle.y && ith->pt_left.x < itv->get_x())
                {
                    action_space as(point(lb_conner.pt),
                                    itv->get_x() - lb_conner.pt.x,
                                    ith->get_y() - lb_conner.pt.y);
                    if (find(g_v_as.begin(), g_v_as.end(), as) == g_v_as.end())
                        g_v_as.push_back(as);
                    break;
                        
                }
            }
        }
        // 如果当前最高的直线下端点y坐标小于lt_conner的y坐标，则屏蔽后面所有直线
        if (vl_min.pt_bottle.y <= lb_conner.pt.y)
            break;
    }
    
}

// 右上角更新算法
void conner2as_rt(const conner & rt_conner)
{
    Vline vl_max;
    // 第一条在rt_conner左边的垂直线，从右到左的顺序
    vector<Vline>::iterator ivl_begin = lower_bound(g_v_vline.begin(),
                                                    g_v_vline.end(),rt_conner.vl);
    if (ivl_begin == g_v_vline.begin() )
        return;
    ivl_begin--;
    // 第一条在rt_conner下面的水平线,从上到下的顺序
    vector<Hline>::iterator ihl_begin = lower_bound(g_v_hline.begin(),
                                                  g_v_hline.end(),rt_conner.hl);
    if (ihl_begin == g_v_hline.begin())
        return ;
    ihl_begin--;

    for (vector<Vline>::iterator itv = ivl_begin; itv != g_v_vline.begin()-1; itv--)
    {
        // 保证右沿线
        if (itv->line_type != RIGHT_LINE)
            continue;
        if (itv->is_higher(vl_max))
            vl_max = *itv;
        else
            continue;
        // 左边界
        if (itv->pt_bottle.y < rt_conner.pt.y)
        {
            //下边界
            for (vector<Hline>::iterator ith = ihl_begin;
                 ith != --g_v_hline.begin(); ith--)
            {
                //保证上沿线
                if (ith->line_type == DOWN_LINE)
                    continue;
                // 水平线在垂直线上部且右端点在垂直线右边，则屏蔽此垂直线
                if (ith->get_y() >= itv->pt_top.y && ith->pt_right.x > itv->get_x())
                    break;
                // 满足下边界
                if (ith->get_y() < itv->pt_top.y && ith->pt_right.x > itv->get_x())
                {
                    action_space as(point(itv->get_x(),ith->get_y()),
                                 rt_conner.pt.x - itv->get_x(),
                                 rt_conner.pt.y - ith->get_y());
                    if (find(g_v_as.begin(), g_v_as.end(), as) == g_v_as.end())
                        g_v_as.push_back(as);
                    break;
                    
                }
            }
        }
        //如果当前最高的直线上端点y坐标大于lt_conner的y坐标，则屏蔽后面所有直线
        if (vl_max.pt_top.y >= rt_conner.pt.y)
            break;
    }
}

// 右下角算法
void conner2as_rb(const conner & rb_conner)
{
    Vline vl_min(point(g_as.left_top()),point(g_as.left_top()),0);
    // 第一条在rb_conner左边的垂直线，从右到左的顺序
    vector<Vline>::iterator ivl_begin = lower_bound(g_v_vline.begin(),
                                                    g_v_vline.end(),rb_conner.vl);
    if (ivl_begin == g_v_vline.begin() )
        return;
    ivl_begin--;

    // 第一条在rb_conner上面的水平线,从下到上的顺序
    vector<Hline>::iterator ihl_begin = upper_bound(g_v_hline.begin(),
                                                  g_v_hline.end(),rb_conner.hl);
    for (vector<Vline>::iterator itv = ivl_begin; itv != g_v_vline.begin()-1; itv--)
    {
        // 保证右沿线
        if (itv->line_type != RIGHT_LINE)
            continue;
        if (itv->is_lower(vl_min))
            vl_min = *itv;
        else
            continue;
        // 左边界
        if (itv->pt_top.y > rb_conner.pt.y)
        {
            // 下边界
            for (vector<Hline>::iterator ith = ihl_begin;
                 ith != g_v_hline.end() ; ++ith)
            {
                //保证下沿线
                if (ith->line_type == UP_LINE)
                    continue;
                // 水平线在垂直线下部且右端点在垂直线右边，则屏蔽此垂直线
                if (ith->get_y() <= itv->pt_bottle.y && ith->pt_right.x > itv->get_x())
                    break;
                // 满足上边界
                if (ith->get_y() > itv->pt_bottle.y && ith->pt_right.x > itv->get_x())
                {
                    action_space as(point(itv->get_x(),rb_conner.pt.y),
                                    rb_conner.pt.x - itv->get_x(),
                                    ith->get_y() - rb_conner.pt.y );
                    if (find(g_v_as.begin(), g_v_as.end(), as) == g_v_as.end())
                        g_v_as.push_back(as);
                    break;
                }
            }
        }
        // 如果当前最高的直线下端点y坐标小于lt_conner的y坐标，则屏蔽后面所有直线
        if (vl_min.pt_bottle.y <= rb_conner.pt.y)
            break;
    }
}

void vline_insert(const Vline & vl )
{
    // g_v_vline 已经有序
    vector<Vline>::iterator ilocv = upper_bound(g_v_vline.begin(),
                                               g_v_vline.end(),vl);
    g_v_vline.push_back(vl);
    vector<Vline>::iterator it = g_v_vline.end()-1;
    for (it; it!= ilocv; --it)
        *it = *(it-1);
    *it = vl;
}

void hline_insert(const Hline & hl)
{
    // g_v_hline 已经有序
    
    vector<Hline>::iterator iloch = upper_bound(g_v_hline.begin(),
                                               g_v_hline.end(),hl);
    g_v_hline.push_back(hl);
    vector<Hline>::iterator ith = g_v_hline.end()-1;
    for (ith; ith!= iloch; --ith)
        *ith = *(ith-1);
    *ith = hl;
}
