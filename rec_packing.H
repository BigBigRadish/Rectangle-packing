#ifndef REC_PACKING_H
#define REC_PACKING_H

// 顶点
struct Conner
{
    int x;
    int y;
    int c_type;
    
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

class Rec_space
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
