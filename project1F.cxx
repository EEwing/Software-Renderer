#include <iostream>
#include <sys/time.h>
#include <pthread.h>

#include <vtkImageData.h>
#include <vtkPNGWriter.h>

#include "reader1F.h"
#include "Screen.h"
#include "Triangle.h"
#include "Vector.h"
#include "LightingParameters.h"

#include <limits>

#include "Camera.h"

#define GET_TIME(now){ \
    struct timeval t; \
    gettimeofday(&t, NULL); \
    now = t.tv_sec + t.tv_usec/1000000.0; \
}

#define NUM_THREADS 8
#define DEBUG true

using std::cerr;
using std::endl;

struct pthread_info {
    std::vector<Triangle> *tris;
    int start, end, totalTris;
    Screen *screen;
    Camera *camera;
    LightingParameters lighting;
};

vtkImageData *
NewImage(int width, int height)
{
    vtkImageData *img = vtkImageData::New();
    img->SetDimensions(width, height, 1);
    img->AllocateScalars(VTK_UNSIGNED_CHAR, 3);

    return img;
}

void
WriteImage(vtkImageData *img, const char *filename)
{
   std::string full_filename = filename;
   full_filename += ".png";
   vtkPNGWriter *writer = vtkPNGWriter::New();
   writer->SetInputData(img);
   writer->SetFileName(full_filename.c_str());
   writer->Write();
   writer->Delete();
}

void *threadRender(void * args) {
   pthread_info *info = (pthread_info *)args;
   int start = info->start;
   int end = info->end;
   int nTriangles = info->totalTris;
   Camera *cam = info->camera;
   LightingParameters lp = info->lighting;
   Screen *screen = info->screen;
   std::vector<Triangle> *triangles = info->tris;
   for(int i=start; i<end; i++) {
      (*triangles)[i].id = i;
      Triangle copy((*triangles)[i]);
      int percent = ((double)(i-start)/(nTriangles-1)) * 100;
      int lastPercent = ((double)(i-1-start)/(nTriangles-1))*100;
      copy.ConvertToDeviceSpace(*screen, *cam);
      copy.render(*screen, *cam, lp);
   }

}

int main()
{
   int width = 1000;
   int height = 1000;
   vtkImageData *image = NewImage(width, height);
   unsigned char *buffer = 
     (unsigned char *) image->GetScalarPointer(0,0,0);
   int npixels = width*height;

   for (int i = 0 ; i < 3*npixels ; i++) {
      buffer[i] = 1;
   }

   std::vector<Triangle> triangles = GetTriangles();

   Screen screen;
   screen.SetSize(width, height);
   screen.buffer = buffer;

   int start = 0;
   int end = triangles.size();

   double nTriangles = end-start;
   fprintf(stderr, "Rendering %d triangles\n", (int)nTriangles);
   LightingParameters lp;
   double startTime, endTime;
   GET_TIME(startTime);
   for(int iter=0; iter<1000; iter++) {

   fprintf(stderr, "\rRunning iteration %d", iter);

   Camera cam = GetCamera(iter, 1000);
   screen.Clear();

   pthread_t threads[NUM_THREADS];
   pthread_info tinfo[NUM_THREADS];
   int trisPerThread = (end-start)/(float)NUM_THREADS;
   int numExtraTris = (end-start) - (trisPerThread * NUM_THREADS);

   int accum = 0;

   for(int i=0; i<NUM_THREADS; ++i) {
       tinfo[i].start = accum;

       if(i < numExtraTris) {
           accum += trisPerThread + 1;
       } else {
           accum += trisPerThread;
       }

       tinfo[i].end = accum;
       tinfo[i].tris = &triangles;
       tinfo[i].screen = &screen;
       tinfo[i].lighting = lp;
       tinfo[i].camera = &cam;
       tinfo[i].totalTris = nTriangles;
       pthread_create(threads+i, NULL, threadRender, tinfo+i);
   }


   for(int i=0; i<NUM_THREADS; ++i) {
       pthread_join(threads[i], NULL);
   }


   char filename[1024];
   sprintf(filename, "Output/camera_%04d", iter);
   WriteImage(image, filename);
   }
   GET_TIME(endTime);
   fprintf(stderr, "\nTotal time: %f, FPS: %f\n", endTime-startTime, 1000/(endTime-startTime));

}
