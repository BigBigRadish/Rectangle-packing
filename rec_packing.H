#ifndef REC_PACKING_H
#define REC_PACKING_H
#include <vector>
#include <algorithm>
using namespace std;

const int LEFT_BOTTLE 0 ;
const int LEFT_TOP 1 ;
const int RIGHT_TOP 2;
const int RIGHT_BOTTLE 3;

// 穴度
class fit_degree
{
public:
    dif_degree():k(0,0,0){}
    bool operator< (const fit_degree & _fd)
        {
            if(k < _fd)
                return 1;
            else if(k == _fd.k)
            {
                if( s < _fd.s)
                    return 1;
                else if(s == _fd.s)
                {
                    if(p < _fd.s)
                        return 1;
                    else
                        return 0;
                }
                else
                    return 0;
            }
            else
                return 0;
        }
    
    bool operator== (const fit_degree & _fd)
        {
            if(k == _fd.k && s == _fd.s && p == _fd.p)
                return 1;
            return 0;
        }
    int k; // 贴边数
    double s; // 平整度
    int p; // 它贴边数
    
};



// 顶点
class conner
{
public:
    conner():x(0),y(0),is_valid(0){}
    conner(int _x , int _y ,int _ctype=0):
        x(_x),y(_y),is_valid(_ctype){}
    void set_conner(int _x ,int _y);

    // 更新角的类型
    void update_conner_type(const vector<rectangle> & vec_rec);

    // 判断点是否在举行rec内部
    bool is_belong2rec(const rectangle & rec);
    
    int x;
    int y;
    // is_valid = 1 实角 ; ctype = 0 虚角
    bool is_valid;
    char conner_type; // 0:left_top; 1:right_top ; 2: left_bottle; 3:right_bottle;
    
};
        

class rectangle
{
public:
    conner left_bottle; // 为了计算方便，把几个点都列出
    int width; // 宽度
    int height; // 高度
    bool reverse_mode ; // 是否旋转：1 旋转; 0未选转

    rectangle():left_bottle(),width(0),height(0),reverse_mode(0){    }

    rectangle(const conner & _left_bottle, int _width, int _height )
        :left_bottle(_left_bottle),width(_width),
         height(_height),reverse_mode(0){    }
    
    rectangle(int _width, int _height )
        :left_bottle(0,0),width(_width),height(_height),reverse_mode(0){    }

    conner left_top()
        {
            return conner(left_bottle.x,left_bottle.y + height);
        }

    conner right_top()
        {
            return conner(left_bottle.x + width, left_bottle.y + height);
        }

    conner right_bottle()
        {
            return conner(left_bottle.x + width, left_bottle.y);
        }

    // 从左下角设置左下角坐标
    void set_ordinate_lb(const conner & _conner_lb)
        {
            left_bottle  = _conner;
        }

    // 从坐上角设置左下角坐标
    void set_ordinate_lt(const conner & _conner_lt)
        {
            left_bottle.x  = _conner_lt.x;
            left_bottle.y  = _conner_lt.y - height;
        }

    // 从右下角设置左下角坐标
    void set_ordinate_rb(const conner & _conner_rb)
        {
            left_bottle.x  = _conner_rb.x - width;
            left_bottle.y  = _conner_lt.y ;
        }

    // 从右上角设置左下角坐标
    void set_ordinate_rt(const conner & _conner_rt)
        {
            left_bottle.x  = _conner_rb.x - width;
            left_bottle.y  = _conner_lt.y - height;
        }

    // 长和宽反转
    void rec_reverse()
        {
            swap(width,height);
            reverse_mode = (reverse_mode + 1)%2;
        }
    

    void set_rectangle(int _lb_x, int _lb_y, int _width , int _height)
        {
            left_bottle.x = _lb_x;
            left_bottle.y = _lb_y;
            right_top.x = _lb_x + _width;
            right_top.y = _lb_y + _height;
        }

    double get_erea() // 面积
        { return width * height ;}

    // 穴度相等,比较其它：面积,长边长，矮边长，左下角x,y
    bool operator> (const rectangle & _rec)
        {
            if(get_erea() > _rec.get_erea())
                return 1;
            else if( get_erea == _rec.get_erea() ) // 面积
            {
                if(max(get_width(),get_height()) >
                   max( _rec.get_width(),_rec.get_height() ))// 最长边
                    return 1;
                else if(max(get_width(),get_height()) ==
                        max( _rec.get_width(),_rec.get_height() ) ) 
                {
                    if( left_bottle.x < _rec.left_bottle.x ) // 左下角x坐标
                        return 1;
                    else if( left_bottle.x == _rec.left_bottle.x) // 左下角y坐标
                    {
                        if( left_bottle.y < _rec.left_bottle.y)
                            return 1;
                        else
                            return 0;
                    }
                    else
                        return 0;
                }
                else
                    return 0;
            }
            else
                return 0;
        }
};
        
class action_space :public rectangle
{
public:
    int place_type; // 放置类型,-1表示还未放置
    bool is_conflict; // 是否被新放入的木块重叠
};


// 判断两个矩形是否冲突
class rectangle_conflict
{
public:
    rectangle_conflict(const conner & _left_bottle,int _width,int _height)
        :left_bottle(_left_bottle),width(_width),height(_height) {}

    // 判断动作空间 ac_space 是否和 矩形块 rec_block 有重叠
    void operator()(action_space & ac_space);
    void set_rectangle(int _lb_x, int _lb_y, int _width,int _height);
private:
    rectangle rec_block;
    
};

    
    

/* 水平线 */
class Hline
{
public:
    conner pt_left; // 左端点
    conner pt_right; // 右端点
    int isblock;

    Hline(const conner & _pt_left,const conner & _pt_right)
        :pt_left(_pt_left),pt_right(_pt_right),isblock(0)
        {
        }
    
    // 长度
    int get_length(){return get_rightx() - get_leftx();   } 

    // 左断点x坐标
    int get_leftx()
        {
            return pt_left.x;
        }

    // 右端点x坐标
    int get_rightx()
        {
            return pt_right.x;
        }

    // y坐标
    int get_y()
        {
            return pt_left.y;
        }
    
    
        
    bool operator< (const Hline & hline1) const
    {
        return y < hline1.y ? 1: 0 ;
    }
    bool operator> (const Hline & hline1) const
    {
        return y > hline1.y ? 1: 0 ;
    }

    bool conflict(const Hline & _hline);
    

};

// 垂直线
class Vline
{
public:
    conner pt_bottle; // 底端点
    conner pt_top; // 顶端点
    int isblock;

    Vline(const conner & _pt_bottle,const conner & _pt_top)
        :pt_bottle(_pt_bottle),pt_top(_pt_top),isblock(0)
        {
        }
    
    // 长度
    int get_length(){return get_topy() - get_bottley();   } 

    // 顶断点y坐标
    int get_bottley()
        {
            return pt_bottle.y;
        }

    // 顶端点y坐标
    int get_topy()
        {
            return pt_top.y;
        }

    // x坐标
    int get_x()
        {
            return pt_top.x;
        }
    
    bool operator< (const Vline & _vline) const
    {
        return get_x() < _vline.get_x() ? 1: 0 ;
    }
    bool operator> (const Vline & _vline) const
    {
        return get_x() > _vline.get_x() ? 1: 0 ;
    }

    bool conflict(const Vline & _vline);

};



     


#endif
