#ifndef REC_PACKING_H
#define REC_PACKING_H
#include <vector>
// 顶点
class conner
{
public:
    conner():x(0),y(0),is_valid(0){}
    conner(int _x , int _y ,int _ctype=0):
        x(_x),y(_y),c(_ctype){}
    void set_conner(int _x ,int _y);

    // 更新角的类型
    void update_conner_type(const vector<rectangle> & vec_rec);
    int x;
    int y;
    // is_valid = 1 实角 ; ctype = 0 虚角
    bool is_valid;
};
        

class rectangle
{
public:
    conner left_bottle;
    conner right_top;
    int get_width()
        {   return right_top.x - left_bottle.x;        }
    int get_height()
        {   return right_top.y - left_bottle.y;        }
    
    rectangle():left_bottle(),right_top(){    }

    rectangle(const conner & _left_bottle, const conner & right_top )
        :left_bottle(_left_bottle),right_top(_right_top){    }

    rectangle(int _lb_x, int _lb_y,int _rt_x, int _rt_y )
        :left_bottle(_lb_x,_lby_y),right_top(_rtx,_rty){}

    void set_rectangle(int _lb_x, int _lb_y, int _width,int _height);
};
        
class action_space :public rectangle
{
public:
    int as_type;
    bool is_conflict;
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
    int x_left;
    int x_right;
    int y;
    int isblock;
    
    Hline(int data_x_left=0,int data_x_right=0,int data_y=0)   \
        :x_right(data_x_right),x_left(data_x_left),y(data_y)
         isblock(0)
        {
        }
        
    bool operator< (const Hline & hline1) const
    {
        return y < hline1.y ? 1: 0 ;
    }
    bool operator> (const Hline & hline1) const
    {
        return y > hline1.y ? 1: 0 ;
    }

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
