#ifndef REC_PACKING_H
#define REC_PACKING_H

// 顶点
class conner
{
public:
    conner(int _x , int _y ,int _ctype):
        x(_x),y(_y),c(_ctype){}
    int getx()
        {            return x;       }
    int gety();
        {            return y;       }
    void setx(int _x)
        {            x = _x;        }
    void sety(int _y)
        {            y = _y;        }
private:
    int x;
    int y;
    int ctype;
};

class rectangle
{
public:
    
private:
    conner left_bottle;
    conner right_top;
}

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
    Conner left_bottle;
    Conner right_top;
    
};

class rec_conflict
{
public:
    
}


     


#endif
