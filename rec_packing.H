#ifndef REC_PACKING_H
#define REC_PACKING_H
#include <vector>
#include <algorithm>
#include <set>

using namespace std;


// conner type
const int LEFT_BOTTLE =  0 ;
const int LEFT_TOP = 1 ;
const int RIGHT_TOP = 2;
const int RIGHT_BOTTLE = 3;


// line type
const int LEFT_LINE = 0 ;
const int RIGHT_LINE = 1 ;
const int UP_LINE = 2 ;
const int DOWN_LINE = 3;


// 穴度
class fit_degree
{
public:
    int k; // 贴边数
    int s; // 平整度
    int j; // 紧凑度
    int p; // 它贴边数
    fit_degree():k(0),s(0),p(0),j(0){}
    bool operator< (const fit_degree & _fd)const
        {
            if(k < _fd.k)
                return 1;
            else if(k == _fd.k)
            {
                if( s < _fd.s)
                    return 1;
                else if(s == _fd.s)
                {
                    if (j < _fd.j)
                        return 1;
                    else if (j == _fd.j)
                    {
                        if(p < _fd.p)
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
    
    bool operator== (const fit_degree & _fd) const
        {
            if(k == _fd.k && s == _fd.s && p == _fd.p)
                return 1;
            return 0;
        }

    
};


class point
{
public:
    int x;
    int y;
    point(int _x ,int _y):x(_x),y(_y){    }
    point(const point &_pt):x(_pt.x),y(_pt.y){    }
    point & operator=(const point& _pt)
    {
        x = _pt.x;
        y = _pt.y;
        return *this;
    }
    bool operator<(const point & _pt) const
    {
        if (x < _pt.x)
            return 1;
        else if( x == _pt.x)
        {
            if(y < _pt.y)
                return 1;
            else
                return 0;
        }
        else
            return 0;
    }
    bool operator==(const point & _pt) const
    {
        if (x == _pt.x && y == _pt.y)
            return 1;
        return 0;
    }
};

/* 水平线 */
class Hline
{
public:
    point pt_left; // 左端点
    point pt_right; // 右端点
    int isblock;
    int line_type;
    Hline():pt_left(0,0),pt_right(0,0){    }
    Hline( point  _pt_left, point  _pt_right,  int   _line_type)
        :pt_left(_pt_left),pt_right(_pt_right),isblock(0),line_type(_line_type){}
    Hline(const Hline & hl):
        pt_left(hl.pt_left),pt_right(hl.pt_right),isblock(hl.isblock),line_type(hl.line_type){}
    // 长度
    int get_length() const {return pt_right.x -pt_left.x   ;} 

    // y坐标
    int get_y() const{return pt_left.y;}

    bool operator== (const Hline & _hline) const
    {
        if (pt_left == _hline.pt_left && pt_right == _hline.pt_right)
            return 1;
        return 0;
    }
    bool operator<  ( const Hline & _hline)  const
     {
         if (get_y() < _hline.get_y() )
             return 1;
         return 0;
     }
    bool operator>   ( const Hline & hline1) const
    {return get_y() > hline1.get_y() ? 1: 0 ;}

    bool conflict( const Hline & _hline) const
     {
         if(get_y() != _hline.get_y())
             return 0;
         int max_x = max(pt_right.x,_hline.pt_right.x);
         int min_x = min(pt_left.x,_hline.pt_left.x);
         if(max_x - min_x < get_length() + _hline.get_length()) // 说明有重叠
             return 1;
         return 0;
     }
    bool is_lefter(const Hline & _hline)const
    {
        if (pt_left.x <= _hline.pt_left.x)
            return 1;
        return 0;
    }
    bool is_righter(const Hline & _hline)const
    {
        if (pt_right.x >= _hline.pt_right.x)
            return 1;
        return 0;
    }
};

// 垂直线
class Vline
{
public:
    point pt_bottle; // 底端点
    point pt_top; // 顶端点
    int isblock;
    int line_type ;
    Vline():pt_bottle(0,0),pt_top(0,0){    }
    Vline( point  _pt_bottle, point  _pt_top,int  _line_type)
        :pt_bottle(_pt_bottle),pt_top(_pt_top),isblock(0),line_type(_line_type){}
    Vline(const Vline & vl)
        :pt_bottle(vl.pt_bottle),pt_top(vl.pt_top),isblock(vl.isblock),line_type(vl.line_type){}
    Vline & operator=(const Vline & _vl)
    {
        pt_bottle = _vl.pt_bottle;
        pt_top = _vl.pt_top;
        isblock = _vl.isblock;
        line_type = _vl.line_type;
    }
    // 长度
    int get_length() const {return pt_top.y - pt_bottle.y;   } 

    // x坐标
    int get_x() const {return pt_top.x;}

    bool operator==(const Vline & _vline) const
    {
        if (pt_top == _vline.pt_top && pt_bottle == _vline.pt_bottle)
            return 1;
        return 0;
    }
    bool operator< ( const Vline & _vline) const
     {
         if (get_x() < _vline.get_x() )
             return 1;
         return 0;
     }
    // bool operator> ( const Vline & _vline)  const
    //     {return get_x() > _vline.get_x() ? 1: 0 ;}

    bool conflict( const Vline & _vline) const
    {
        if(get_x() != _vline.get_x())
            return 0;
        int max_y = max(pt_top.y,_vline.pt_top.y);
        int min_y = min(pt_bottle.y, _vline.pt_bottle.y);
        if (max_y - min_y < get_length() + _vline.get_length() )
            return 1;
        return 0;
    }

    bool is_higher(const Vline & _vline) const
    {
        if (pt_top.y >= _vline.pt_top.y)
            return 1;
        return 0;
    }

    bool is_lower(const Vline & _vline) const
    {
        if (pt_bottle.y <= _vline.pt_bottle.y)
            return 1;
        return 0;
    }
};

// 顶点
class conner
{
public:
    // is_valid = 1 实角 ; is_balid = 0 虚角
    bool is_valid;
    int  conner_type; //角类型
    point pt; // 角坐标
    Vline vl;
    Hline hl;
    conner(int _x , int _y ,const Vline &_vl,const Hline & _hl,
           int _is_valid=0, int _conner_type = -1):
        pt(_x,_y),is_valid(_is_valid),conner_type(_conner_type),
        vl(_vl),hl(_hl){}
    conner(const conner & _cn)
        :pt(_cn.pt),is_valid(_cn.is_valid),conner_type(_cn.conner_type),
         vl(_cn.vl),hl(_cn.hl){}
    conner(const point & _pt):pt(_pt),is_valid(0),conner_type(-1){}
    conner(int _x , int _y,int _is_valid=0, int _conner_type = -1 ):
        pt(_x,_y),is_valid(_is_valid),conner_type(_conner_type){    }
    bool operator==(const conner & _cn)
        {
            return pt == _cn.pt ;
        }
    conner & operator=(const conner & _cn)
    {
        pt = _cn.pt;
        is_valid = _cn.is_valid;
        conner_type = _cn.conner_type;
        vl = _cn.vl;
        hl = _cn.hl;
        return *this;
    }
    bool operator <(const conner & _cn) const
     {
         return pt < _cn.pt ;
         // if (pt< _cn.pt)
         //     return 1;
         // else if (pt == _cn.pt)
         //     return conner_type < _cn.conner_type;
         // else
         //     return 0;
     }
};

class rectangle
{
public:
    point left_bottle; // 左下角
    int width; // 宽度
    int height; // 高度
    bool reverse_mode ; // 是否旋转：1 旋转; 0未选转
     int place_type; // 放置类型,-1表示还未放置
                     // 取值 LEFT_BOTTLE,LEFT_TOP,RIGHT_BOTTLE, RIGHT_TOP
    int time; // 记录时间
    int id; // 记录编号
    
    rectangle():left_bottle(0,0),width(0),height(0),
                reverse_mode(0),place_type(-1),time(0),id(-1){    }

    rectangle(const point & _left_bottle, int _width, int _height )
        :left_bottle(_left_bottle),width(_width),
         height(_height),reverse_mode(0){    }
    rectangle(const point & _left_bottle, int _width,
              int _height,int _place_type)
        :left_bottle(_left_bottle),width(_width),
         height(_height),reverse_mode(0),place_type(_place_type){    }
    
    rectangle(int _width, int _height )
        :left_bottle(0,0),width(_width),height(_height),reverse_mode(0),id(-1){    }

    rectangle(const rectangle & _rec)
        :left_bottle(_rec.left_bottle),width(_rec.width),height(_rec.height),
         reverse_mode(_rec.reverse_mode),
         place_type(_rec.place_type),time(_rec.time),id(_rec.id)
        {    }

    rectangle & operator=(const rectangle & _rec)
    {
        left_bottle = _rec.left_bottle;
        width = _rec.width;
        height = _rec.height;
        reverse_mode = _rec.reverse_mode ;
        place_type = _rec.place_type;
        time = _rec.time;
        id = _rec.id;
        return *this;
    }

    point  left_top() const
        {
            return point(left_bottle.x,left_bottle.y + height);
        }

    point  right_top() const
        {
            return point(left_bottle.x + width, left_bottle.y + height);
        }

    point  right_bottle() const
        {
            return point(left_bottle.x + width, left_bottle.y);
        }

    // 从左下角设置左下角坐标
    void set_ordinate_lb(const point & _point_lb)
        {
            left_bottle  = _point_lb;
            place_type = LEFT_BOTTLE;
        }

    // 从坐上角设置左下角坐标
    void set_ordinate_lt(const point & _point_lt)
        {
            left_bottle.x  = _point_lt.x;
            left_bottle.y  = _point_lt.y - height;
            place_type = LEFT_TOP;
        }

    // 从右下角设置左下角坐标
    void set_ordinate_rb(const point & _point_rb)
        {
            left_bottle.x  = _point_rb.x - width;
            left_bottle.y  = _point_rb.y ;
            place_type = RIGHT_BOTTLE;
        }

    // 从右上角设置左下角坐标
    void set_ordinate_rt(const point & _point_rt)
        {
            left_bottle.x  = _point_rt.x - width;
            left_bottle.y  = _point_rt.y - height;
            place_type = RIGHT_TOP;
        }

    // 长和宽反转
    void rec_reverse()
        {
            swap(width,height);
            reverse_mode = (reverse_mode + 1)%2;
        }
    
    int  get_area() const // 面积
        { return width * height ;}

    
    // 穴度相等,比较其它：面积,长边长，矮边长，左下角x,y
    // trur表示 优于 _rec
    bool operator> (const rectangle & _rec)const
        {
            if(get_area() > _rec.get_area())
                return 1;
            else if( get_area() == _rec.get_area() ) // 面积
            {
                if(max(width,height ) >
                   max( _rec.width,_rec.height ))// 最长边
                    return 1;
                else if(max(width,height) ==
                        max( _rec.width,_rec.height ) ) 
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
    bool operator==(const rectangle & _rec) const
    {
        if (min(width,height) == min(_rec.width,_rec.height)
            && max(width,height) == max(_rec.width,_rec.height)
            )
            return 1;
        else
            return 0;
    }
    bool is_equal_withtime(const rectangle & _rec) const
        {
            if (*this == _rec && time == _rec.time)
                return 1;
            else
                return 0;
        }
    bool operator<( const rectangle & _rec) const
        {
            if (*this == _rec || *this > _rec)
                return 0;
            else
                return 1;
        }
};
        
class action_space :public rectangle
{
public:
    bool is_conflict; // 是否被新放入的木块重叠

    action_space():rectangle(0,0){    }
    action_space(const point & _left_bottle, int _width=0, int _height=0):
        rectangle(_left_bottle,_width,_height),is_conflict(0){    }
    action_space(const action_space & as):
        rectangle(as.left_bottle,as.width,as.height,as.place_type),
        is_conflict(as.is_conflict){    }
    
    action_space & operator=(const action_space & as)
        {
            left_bottle = as.left_bottle;
            width = as.width;
            height = as.height;
            place_type = as.place_type;
        }
    
    bool operator==(const action_space & _as) const
        {
            if (left_bottle == _as.left_bottle &&
                width == _as.width &&  height == _as.height)
                return 1;
            else
                return 0;
        }
    bool operator<(const action_space & _as) const
    {
        if(left_bottle < _as.left_bottle )
            return 1;
        else if (left_bottle == _as.left_bottle)
        {
            if (width < _as.width)
                return 1;
            else if (width == _as.width)
            {
                if (height < _as.height)
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
};


// 判断两个矩形是否冲突
class rectangle_conflict
{
public:
    rectangle_conflict(const point & _left_bottle,int _width,int _height)
        :rec_block(_left_bottle,_width,_height){    }
    
    // 判断动作空间 ac_space 是否和 矩形块 rec_block 有重叠
    void operator()(action_space & ac_space)
        {
            int minx = max(ac_space.left_bottle.x , rec_block.left_bottle.x);
            int maxx = min(ac_space.left_bottle.x+ac_space.width ,
                           rec_block.left_bottle.x+ rec_block.width );
            int miny = max(ac_space.left_bottle.y , rec_block.left_bottle.y);
            int maxy = min(ac_space.left_bottle.y+ac_space.height ,
                           rec_block.left_bottle.y + rec_block.height);

            if( minx >= maxx || miny >= maxy)// not conflict
                ac_space.is_conflict = 0;
            else                          // action space ac_space conflict 
                ac_space.is_conflict = 1; // with rec_block                 
        }
private:
    rectangle rec_block;
    
};



// 占角动作优先考虑时间
// 时间越大，越优先
class conner_action
{
public:
    fit_degree fd;
    vector<rectangle>::iterator irec;
    vector<action_space>::iterator ias;
    conner_action(const fit_degree & _fd,vector<rectangle>::iterator _irec,vector<action_space>::iterator  _ias)
        :fd(_fd),irec(_irec),ias(_ias){    }
    bool operator<(const conner_action & _ac)const
        {
            if (irec->time < _ac.irec->time)
                return 1;
            else if (irec->time == _ac.irec->time)
            {
                if (fd < _ac.fd)
                    return 1;
                else if (fd == _ac.fd)
                {
                    if (*irec < *(_ac.irec))
                        return 1;
                    else
                        return 0 ;
                }
                else
                    return 0;
            }
            else
                return 0;
        }
    // bool operator<(const conner_action & _ac)const
    //     {
    //         if (fd < _ac.fd)
    //             return 1;
    //         else if (fd == _ac.fd)
    //         {
    //             if (*irec < *(_ac.irec))
    //                 return 1;
    //             else if(*irec == *(_ac.irec))
    //             {
    //                 if (irec->time < _ac.irec->time)
    //                     return 1;
    //                 else
    //                     return 0;
    //             }
    //             else
    //                 return 0 ;
    //         }
    //         else
    //             return 0;
    //     }
    
    bool operator ==(const conner_action &_ac)
        {
            if (fd == _ac.fd && irec->is_equal_withtime(*(_ac.irec))  && *ias == *(_ac.ias))
                return 1;
            else
                return 0;
        }
    conner_action & operator=(const conner_action & _ac)
    {
        fd = _ac.fd;
        irec = _ac.irec;
        ias = _ac.ias;
        return *this;
    }
    
};

#endif
