 /*****************
 
 Wesley Ferreira Marques - wesley.marques@gmail.com 
 http://codevein.com
 
 This port was based in a inital code from Jesus Gollonet, him port Penners easing equations to C/C++:
 
 http://www.jesusgollonet.com/blog/2007/09/24/penner-easing-cpp/
 http://robertpenner.com/easing/
 
 I'm just make a better wrapper a litlle more OOP e put some callbacks like the original Tweener
 (http://code.google.com/p/tweener/)
 
 **********************/

#ifndef _CPP_TWEEEN_
#define _CPP_TWEEEN_

#include "KQueryTweenDef.h"

#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

/*
Linear：无缓动效果；
Quadratic：二次方的缓动（t^2）；
Cubic：三次方的缓动（t^3）；
Quartic：四次方的缓动（t^4）；
Quintic：五次方的缓动（t^5）；
Sinusoidal：正弦曲线的缓动（sin(t)）；
Exponential：指数曲线的缓动（2^t）；
Circular：圆形曲线的缓动（sqrt(1-t^2)）；
Elastic：指数衰减的正弦曲线缓动；
Back：超过范围的三次方缓动（(s+1)*t^3 - s*t^2）；
Bounce：指数衰减的反弹缓动。

easeIn：从0开始加速的缓动；
easeOut：减速到0的缓动；
easeInOut：前半段从0开始加速，后半段减速到0的缓动。


参数  t:Number — 指定当前时间，介于 0 和持续时间之间（包括二者）。  
b:Number — 指定动画属性的初始值。  
c:Number — 指定动画属性的更改总计。  
d:Number — 指定运动的持续时间。  
s:Number (default = 0) — 指定过冲量，此处数值越大，过冲越大。  
Sets and returns the overshoot for the Back tween.  Where the higher the value, the greater the overshoot.=2

a,p - amplitude and period [0 , 1] 
a     Sets and returns the amplitude (how much) of the sine wave for the Elastic tween.  =0.7
p     Sets and returns the period (how often) of the sine wave for the Elastic tween.  = 0.3
*/

#pragma warning(push)  
#pragma warning(disable : 4244)

typedef float (* TweenFuncType ) (float t,float b , float c, float d, float p0, float p1);
    /***** LINEAR ****/
float Linear__easeNone (float t,float b , float c, float d, float p0, float p1) {
        return c*t/d + b;
}
float Linear__easeIn (float t,float b , float c, float d, float p0, float p1) {
        return c*t/d + b;
}
float Linear__easeOut(float t,float b , float c, float d, float p0, float p1) {
        return c*t/d + b;
}

float Linear__easeInOut(float t,float b , float c, float d, float p0, float p1) {
        return c*t/d + b;
}

/***** SINE ****/

float Sine__easeIn (float t,float b , float c, float d, float p0, float p1) {
        return -c * cos(t/d * (PI/2)) + c + b;
}
float Sine__easeOut(float t,float b , float c, float d, float p0, float p1) {
        return c * sin(t/d * (PI/2)) + b;
}

float Sine__easeInOut(float t,float b , float c, float d, float p0, float p1) {
        return -c/2 * (cos(PI*t/d) - 1) + b;
}

/**** Quint ****/

float Quint__easeIn (float t,float b , float c, float d, float p0, float p1) {
        return c*(t/=d)*t*t*t*t + b;
}
float Quint__easeOut(float t,float b , float c, float d, float p0, float p1) {
        return c*((t=t/d-1)*t*t*t*t + 1) + b;
}

float Quint__easeInOut(float t,float b , float c, float d, float p0, float p1) {
        if ((t/=d/2) < 1) return c/2*t*t*t*t*t + b;
        return c/2*((t-=2)*t*t*t*t + 2) + b;
}

/**** Quart ****/
float Quart__easeIn (float t,float b , float c, float d, float p0, float p1) {
        return c*(t/=d)*t*t*t + b;
}
float Quart__easeOut(float t,float b , float c, float d, float p0, float p1) {
        return -c * ((t=t/d-1)*t*t*t - 1) + b;
}

float Quart__easeInOut(float t,float b , float c, float d, float p0, float p1) {
        if ((t/=d/2) < 1) return c/2*t*t*t*t + b;
        return -c/2 * ((t-=2)*t*t*t - 2) + b;
}

/**** Quad ****/
float Quad__easeIn (float t,float b , float c, float d, float p0, float p1) {
        return c*(t/=d)*t + b;
}
float Quad__easeOut(float t,float b , float c, float d, float p0, float p1) {
        return -c *(t/=d)*(t-2) + b;
}

float Quad__easeInOut(float t,float b , float c, float d, float p0, float p1) {
        if ((t/=d/2) < 1) return ((c/2)*(t*t)) + b;
        return -c/2 * (((t-2)*(--t)) - 1) + b;
        /*
         originally return -c/2 * (((t-2)*(--t)) - 1) + b;

         I've had to swap (--t)*(t-2) due to diffence in behaviour in
         pre-increment operators between java and c++, after hours
         of joy
         */

}

/**** Expo ****/

float Expo__easeIn (float t,float b , float c, float d, float p0, float p1) {
        return (t==0) ? b : c * pow(2, 10 * (t/d - 1)) + b;
}
float Expo__easeOut(float t,float b , float c, float d, float p0, float p1) {
        return (t==d) ? b+c : c * (-pow(2, -10 * t/d) + 1) + b;
}

float Expo__easeInOut(float t,float b , float c, float d, float p0, float p1) {
        if (t==0) return b;
        if (t==d) return b+c;
        if ((t/=d/2) < 1) return c/2 * pow(2, 10 * (t - 1)) + b;
        return c/2 * (-pow(2, -10 * --t) + 2) + b;
}


/****  Elastic ****/

float Elastic__easeIn (float t, float b, float c, float d, float a, float p) {
        if (t==0) return b;  if ((t/=d)==1) return b+c;
        if (-1 == p) p=d*.3;
        else p = d*p;
        float s=p/4;
        if (-1 == a || a < abs(c)) { a=c; }
        else {s = p/(2*PI) * asin (c/a);}
        //float p=d*.3f;
        //float a=c;
        
        float postFix =a*pow(2,10*(t-=1)); // this is a fix, again, with post-increment operators
        return -(postFix * sin((t*d-s)*(2*PI)/p )) + b;
}

float Elastic__easeOut(float t, float b, float c, float d, float a, float p) {
        if (t==0) return b;
        if ((t/=d)==1) return b+c;
        if (-1 == p) p=d*.3;
        else p = d*p;
        float s=p/4;
        if (-1 == a || a < abs(c)) { a=c; }
        else {s = p/(2*PI) * asin (c/a);}
        //float p=d*.3f;
        //float a=c;
        //float s=p/4;
        return (a*pow(2,-10*t) * sin( (t*d-s)*(2*PI)/p ) + c + b);
}

float Elastic__easeInOut(float t, float b, float c, float d, float a, float p) {
        if (t==0) return b;  if ((t/=d/2)==2) return b+c;
        if (-1 == p) p=d*(.3f*1.5f);
        else p = d*p;
        float s=p/4;
        if (-1 == a || a < abs(c)) { a=c; }
        else {s = p/(2*PI) * asin (c/a);}
        //float p=d*(.3f*1.5f);
        //float a=c;
        //float s=p/4;

        if (t < 1) {
                float postFix =a*pow(2,10*(t-=1)); // postIncrement is evil
                return -.5f*(postFix* sin( (t*d-s)*(2*PI)/p )) + b;
        }
        float postFix =  a*pow(2,-10*(t-=1)); // postIncrement is evil
        return postFix * sin( (t*d-s)*(2*PI)/p )*.5f + c + b;
}

/****  Cubic ****/
float Cubic__easeIn (float t,float b , float c, float d, float p0, float p1) {
        return c*(t/=d)*t*t + b;
}
float Cubic__easeOut(float t,float b , float c, float d, float p0, float p1) {
        return c*((t=t/d-1)*t*t + 1) + b;
}

float Cubic__easeInOut(float t,float b , float c, float d, float p0, float p1) {
        if ((t/=d/2) < 1) return c/2*t*t*t + b;
        return c/2*((t-=2)*t*t + 2) + b;
}

/*** Circ ***/

float Circ__easeIn (float t,float b , float c, float d, float p0, float p1) {
        return -c * (sqrt(1 - (t/=d)*t) - 1) + b;
}
float Circ__easeOut(float t,float b , float c, float d, float p0, float p1) {
        return c * sqrt(1 - (t=t/d-1)*t) + b;
}

float Circ__easeInOut(float t,float b , float c, float d, float p0, float p1) {
        if ((t/=d/2) < 1) return -c/2 * (sqrt(1 - t*t) - 1) + b;
        return c/2 * (sqrt(1 - t*(t-=2)) + 1) + b;
}

/****  Bounce ****/
float Bounce__easeOut(float t,float b , float c, float d, float p0, float p1) {
    if ((t/=d) < (1/2.75f)) {
            return c*(7.5625f*t*t) + b;
    } else if (t < (2/2.75f)) {
            float postFix = t-=(1.5f/2.75f);
            return c*(7.5625f*(postFix)*t + .75f) + b;
    } else if (t < (2.5/2.75)) {
            float postFix = t-=(2.25f/2.75f);
            return c*(7.5625f*(postFix)*t + .9375f) + b;
    } else {
            float postFix = t-=(2.625f/2.75f);
            return c*(7.5625f*(postFix)*t + .984375f) + b;
    }
}

float Bounce__easeIn (float t,float b , float c, float d, float p0, float p1) {
    return c - Bounce__easeOut (d-t, 0, c, d, p0, p1) + b;
}

float Bounce__easeInOut(float t,float b , float c, float d, float p0, float p1) {
    if (t < d/2) return Bounce__easeIn (t*2, 0, c, d, p0, p1) * .5f + b;
    else return Bounce__easeOut (t*2-d, 0, c, d, p0, p1) * .5f + c*.5f + b;
}

/**** Back *****/

float Back__easeIn (float t,float b , float c, float d, float s, float p1) {
    if (-1 == s) {s = 1.70158f;}
    //float s = 1.70158f;
    float postFix = t/=d;
    return c*(postFix)*t*((s+1)*t - s) + b;
}
float Back__easeOut(float t,float b , float c, float d, float s, float p1) {
    //float s = 1.70158f;
    if (-1 == s) {s = 1.70158f;}
    return c*((t=t/d-1)*t*((s+1)*t + s) + 1) + b;
}

float Back__easeInOut(float t,float b , float c, float d, float s, float p1) {
        //float s = 1.70158f;
    if (-1 == s) {s = 1.70158f;}
    if ((t/=d/2) < 1) return c/2*(t*t*(((s*=(1.525f))+1)*t - s)) + b;
    float postFix = t-=2;
    return c/2*((postFix)*t*(((s*=(1.525f))+1)*t + s) + 2) + b;
}

TweenFuncType KqGetTween(KQueryTween tweenType, KQueryEasing easingType) {
    static bool init = false;
    static TweenFuncType g_TweenFuncArr[11][3];
    if (false == init) {
        init = true;
        g_TweenFuncArr[KQT_LINEAR][KQE_EASE_IN] = &Linear__easeIn;
        g_TweenFuncArr[KQT_LINEAR][KQE_EASE_OUT] = &Linear__easeOut;
        g_TweenFuncArr[KQT_LINEAR][KQE_EASE_IN_OUT] = &Linear__easeInOut;

        g_TweenFuncArr[KQT_SINE][KQE_EASE_IN]  = &Sine__easeIn;
        g_TweenFuncArr[KQT_SINE][KQE_EASE_OUT]  = &Sine__easeOut;
        g_TweenFuncArr[KQT_SINE][KQE_EASE_IN_OUT]  = &Sine__easeInOut;

        g_TweenFuncArr[KQT_QUINT][KQE_EASE_IN] = &Quint__easeIn;
        g_TweenFuncArr[KQT_QUINT][KQE_EASE_OUT] = &Quint__easeOut;
        g_TweenFuncArr[KQT_QUINT][KQE_EASE_IN_OUT] = &Quint__easeInOut;

        g_TweenFuncArr[KQT_QUART][KQE_EASE_IN] = &Quart__easeIn;
        g_TweenFuncArr[KQT_QUART][KQE_EASE_OUT] = &Quart__easeOut;
        g_TweenFuncArr[KQT_QUART][KQE_EASE_IN_OUT] = &Quart__easeInOut;

        g_TweenFuncArr[KQT_QUAD][KQE_EASE_IN] = &Quad__easeIn;
        g_TweenFuncArr[KQT_QUAD][KQE_EASE_OUT] = &Quad__easeOut;
        g_TweenFuncArr[KQT_QUAD][KQE_EASE_IN_OUT] = &Quad__easeInOut;

        g_TweenFuncArr[KQT_EXPO][KQE_EASE_IN] = &Expo__easeIn;
        g_TweenFuncArr[KQT_EXPO][KQE_EASE_OUT] = &Expo__easeOut;
        g_TweenFuncArr[KQT_EXPO][KQE_EASE_IN_OUT] = &Expo__easeInOut;

        g_TweenFuncArr[KQT_ELASTIC][KQE_EASE_IN] = &Elastic__easeIn;
        g_TweenFuncArr[KQT_ELASTIC][KQE_EASE_OUT] = &Elastic__easeOut;
        g_TweenFuncArr[KQT_ELASTIC][KQE_EASE_IN_OUT] = &Elastic__easeInOut;

        g_TweenFuncArr[KQT_CUBIC][KQE_EASE_IN] = &Cubic__easeIn;
        g_TweenFuncArr[KQT_CUBIC][KQE_EASE_OUT] = &Cubic__easeOut;
        g_TweenFuncArr[KQT_CUBIC][KQE_EASE_IN_OUT] = &Cubic__easeInOut;

        g_TweenFuncArr[KQT_CIRC][KQE_EASE_IN] =  &Circ__easeIn;
        g_TweenFuncArr[KQT_CIRC][KQE_EASE_OUT] =  &Circ__easeOut;
        g_TweenFuncArr[KQT_CIRC][KQE_EASE_IN_OUT] =  &Circ__easeInOut;

        g_TweenFuncArr[KQT_BOUNCE][KQE_EASE_IN] =  &Bounce__easeIn;
        g_TweenFuncArr[KQT_BOUNCE][KQE_EASE_OUT] =  &Bounce__easeOut;
        g_TweenFuncArr[KQT_BOUNCE][KQE_EASE_IN_OUT] =  &Bounce__easeInOut;

        g_TweenFuncArr[KQT_BACK][KQE_EASE_IN] =  &Back__easeIn;
        g_TweenFuncArr[KQT_BACK][KQE_EASE_OUT] =  &Back__easeOut;
        g_TweenFuncArr[KQT_BACK][KQE_EASE_IN_OUT] =  &Back__easeInOut;
    }

    return g_TweenFuncArr[tweenType][easingType];
}

#pragma warning(pop)

#endif