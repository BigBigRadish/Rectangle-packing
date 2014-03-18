#ifndef REC_PACKING_H
#define REC_PACKING_H
#include <vector>
#include <algorithm>

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
    int s; // 平整度
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
    conner right_top;
    conner left_top;
    conner right_bottle;
    int width;
    int height;
    

    conner get_lefttop()
        {
            return conner(left_bottle.x,left_bottle.y + height);
        }

    conner get_righttop()
        {
            return conner(left_bottle.x + width, left_bottle.y + height);
        }

    conner get_rightbottle()
        {
            return conner(left_bottle.x + width, left_bottle.y);
        }
    
    int get_width() // 宽度
        {   return right_top.x - left_bottle.x;        }
    int get_height() // 高度
        {   return right_top.y - left_bottle.y;        }
    
    rectangle():left_bottle(),right_top(){    }

    rectangle(const conner & _left_bottle, const conner & right_top )
        :left_bottle(_left_bottle),right_top(_right_top){    }

    rectangle(int _lb_x, int _lb_y,int _rt_x, int _rt_y )
        :left_bottle(_lb_x,_lby_y),right_top(_rtx,_rty){}

    void set_rectangle(int _lb_x, int _lb_y, int _width,int _height);

    double get_erea() // 面积
        { return get_width() * get_height() ;}

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
    int as_type; // 动作空间类型
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
    
l    bool operator< (const Vline & _vline) const
    {
        return get_x() < _vline.get_x() ? 1: 0 ;
    }
    bool operator> (const Vline & _vline) const
    {
        return get_x() > _vline.get_x() ? 1: 0 ;
    }

    bool conflict(const Vline & _vline);

};



// class Vline
// {
// public:
//     int y_top;
//     int y_bottle;
//     int x;
//     Vline():
//         {
//         }
    
    
// };

class rectangle
{
public:
    conner left_bottle;
    conner right_top;
    
};



     


#endif
