#include "plot.h"

//画图时原点与范围转换
void GLCoordinatesTransform(const double *a,const double *b,const double *c,GLfloat *vertices,int maxwide,int length) {
    double amin,amax,bmin,bmax,cmin,cmax,acenter,bcenter,ccenter;
    amin=FindMinOrMax(false,a,length);
    bmin=FindMinOrMax(false,b,length);
    cmin=FindMinOrMax(false,c,length);
    amax=FindMinOrMax(true,a,length);
    bmax=FindMinOrMax(true,b,length);
    cmax=FindMinOrMax(true,c,length);
    acenter=(amax+amin)/2;
    bcenter=(bmax+bmin)/2;
    ccenter=(cmax+cmin)/2;
    double wide[3]={amax-amin,bmax-bmin,cmax-cmin},scale;
    scale=maxwide/FindMinOrMax(true,wide,3);
    for(int i=0;i<length;i++) {
        vertices[i*3]= static_cast<GLfloat>((a[i] - acenter) * scale);
        vertices[i*3+1]= static_cast<GLfloat>((b[i] - bcenter) * scale);
        vertices[i*3+2]= static_cast<GLfloat>((c[i] - ccenter) * scale);
    }
}
void GlPlot3D::plot() {
    double h=tmax/(minstep-1),r1[3],r2[3],rtmax=1/tmax;
    double4 cache,cache1,cache2;
    step=minstep;
    try {
        for(int i=0;i<step;i++) {
            cache.v[0]=h*i;
            cache.v[1]=(*fx)(cache.v[0]);
            cache.v[2]=(*fy)(cache.v[0]);
            cache.v[3]=(*fz)(cache.v[0]);
            data.push_back(cache);
        }
    } catch(std::out_of_range) {}
    dataIterator=data.begin();
    cache1=*(dataIterator);
    dataIterator++;
    cache2=*(dataIterator);
    vminus(&(cache2.v[1]),&(cache1.v[1]),r2,3);
    dataIterator++;
    for(;dataIterator!=data.end();dataIterator++) {
        vequ(r2,r1,3);
        vminus(&(dataIterator->v[1]),&(cache2.v[1]),r2,3);
        if(!vCosTheta3(r1,r2)) {
            cache.v[0]=((*(dataIterator)).v[0]+cache2.v[0])*0.5;
            cache.v[1]=(*fx)(cache.v[0]);
            cache.v[2]=(*fy)(cache.v[0]);
            cache.v[3]=(*fz)(cache.v[0]);
            data.insert(dataIterator,cache);
            dataIterator--;
            dataIterator--;
            cache.v[0]=(cache1.v[0]+cache2.v[0])*0.5;
            cache.v[1]=(*fx)(cache.v[0]);
            cache.v[2]=(*fy)(cache.v[0]);
            cache.v[3]=(*fz)(cache.v[0]);
            data.insert(dataIterator,cache);
            cache2=cache;
            vminus(&(cache2.v[1]),&(cache1.v[1]),r2,3);
            dataIterator--;
            step+=2;
        }
        else {
            cache1=cache2;
            cache2=*(dataIterator);
        }
    }
    vertices=new double[3*step];
    normedT=new GLfloat[step];
    dataIterator=data.begin();
    double a[step],b[step],c[step];
    for(int i=0;i<step;i++) {
        normedT[i]=(*(dataIterator)).v[0]*rtmax;
        a[i]=(*(dataIterator)).v[1];
        b[i]=(*(dataIterator)).v[2];
        c[i]=(*(dataIterator)).v[3];
        dataIterator++;
    }
   setVertices(a,b,c);
   delete []Vbo;
   Vbo=new GLfloat[6*step];
   for(int i=0;i<step;i++) {
       Vbo[i*6+3]= 0.0f;
       Vbo[i*6+4]= 1.0f;
       Vbo[i*6+5]= 0.0f;
   }
}
GlPlot3D::GlPlot3D(std::function<double(double)> &ifx,std::function<double(double)> &ify,std::function<double(double)> &ifz,double itmax,int imaxwide,int iminstep,double iminCosTheta) {
    fx=&ifx;
    fy=&ify;
    fz=&ifz;
    tmax=itmax;
    maxwide=imaxwide;
    minstep=iminstep;
    minCosTheta=iminCosTheta;
};
int GlPlot3D::Step() {
    return step;
};
bool GlPlot3D::vCosTheta3(const double * r1,const double *r2) {
    return (vdot(r1,r2,3)/(sqrt(vdot(r1,r1,3))*sqrt(vdot(r2,r2,3)))>minCosTheta);
}
void GlPlot3D::setVertices(double *a,double *b,double *c) {
    for(int i=0;i<step;i++) {
        vertices[i*3]= a[i];
        vertices[i*3+1]= b[i];
        vertices[i*3+2]= c[i];
    }
};
GLfloat glFindMinOrMax(bool Max,const GLfloat *a,int length,int step) {
    GLfloat cache=a[0];
    for(int i=1;i<length;i++) {
        if((Max ? a[i*step]>cache : a[i*step]<cache))
            cache=a[i*step];
    }
    return cache;
}
std::vector<double> GlPlot3D::getScaleAndOrigin() {
    double amin,amax,bmin,bmax,cmin,cmax,acenter,bcenter,ccenter;
    amin=FindMinOrMax(false,&vertices[0],step,3);
    bmin=FindMinOrMax(false,&vertices[1],step,3);
    cmin=FindMinOrMax(false,&vertices[2],step,3);
    amax=FindMinOrMax(true,&vertices[0],step,3);
    bmax=FindMinOrMax(true,&vertices[1],step,3);
    cmax=FindMinOrMax(true,&vertices[2],step,3);
    acenter=(amax+amin)/2;
    bcenter=(bmax+bmin)/2;
    ccenter=(cmax+cmin)/2;
    double wide[3]={amax-amin,bmax-bmin,cmax-cmin},scale;
    scale=maxwide/FindMinOrMax(true,wide,3);
    std::vector<double> result(4);
    result[0]=scale;
    result[1]=acenter;
    result[2]=bcenter;
    result[3]=ccenter;
    return result;
};
void GlPlot3D::shift(std::vector<double> in) {
    for(int i=0;i<step;i++) {
        Vbo[i*6]= GLfloat((vertices[i*3]-in[1])*in[0]);
        Vbo[i*6+1]= GLfloat((vertices[i*3+1]-in[2])*in[0]);
        Vbo[i*6+2]= GLfloat((vertices[i*3+2]-in[3])*in[0]);
    }
}
