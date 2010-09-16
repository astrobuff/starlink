/*
*+
*  Name:
*     fts2_deglitch.c

*  Purpose:
*     Removes the glitches (if any) from the given interferogram. 

*  Language:
*     Starlink ANSI C

*  Type of Module:

*  Invocation:

*  Description:
*    Removes the glitches (if any) from the given interferogram.

*  Authors:
*     Coskun (Josh) OBA (UoL)

*  History :
*     2010-08-26 (COBA):
*        Original.

*  Copyright:
*     Copyright (C) 2010 Science and Technology Facilities Council.
*     Copyright (C) 2010 University of Lethbridge. All Rights Reserved.

*  License:
*     This program is free software; you can redistribute it and/or
*     modify it under the terms of the GNU General Public License as
*     published by the Free Software Foundation; either version 3 of
*     the License, or (at your option) any later version.
*
*     This program is distributed in the hope that it will be
*     useful, but WITHOUT ANY WARRANTY; without even the implied
*     warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
*     PURPOSE. See the GNU General Public License for more details.
*
*     You should have received a copy of the GNU General Public
*     License along with this program; if not, write to the Free
*     Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
*     MA 02111-1307, USA

*  Bugs:
*     {note_any_bugs_here}
*-
*/

/* STANDARD INCLUDES */
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

/* STARLINK INCLUDES */
#include "ast.h"

/* SMURF INCLUDES */
#include "fts2.h"

void fts2_deglitch(
    double* interferogram,              /* The interferogram */
    int size,                           /* Sample size of the interferogram */
    int coreClusterSize,                /* Core cluster size */
    int tailClusterSize,                /* Tail cluster size */
    double tailCutoffStdDevPercent,     /* Tail cutoff standard deviation (as percentage) */
    double tailCutoffStdDevMultiplier,  /* Tail cutoff standard deviation multiplier */
    int zpdIndex,                       /* Index of ZPD */
    int dsHalfLength,                   /* Size of the double sided interferogram */
    smf_deglitchmode mode,			        /* Deglitch mode */
    double threshold)             	    /* Deglitch threshold */
{
  int end                 = 0;
  int i                   = 0;
  int j                   = 0;
  int numCluster          = 0;
  int start               = 0;
  int tailDeglitchStart   = 0;
  double avgSigma         = 0.0;
  double coreLength       = 0.0;
  double cutoffAmplitude  = 0.0;
  double cutoffFreq       = 0.0;
  double cutoffSigma      = 0.0;
  double error            = 0.0;
  double max              = DBL_MAX;
  double maxSigma         = 0.0;
  double mean             = 0.0;
  double min              = DBL_MIN;
  double tForward         = 0.0;
  double tReverse         = 0.0;
  double tmp              = 0.0;
  double revCoreLength    = 0.0;
  double y                = 0.0;
  double* clusterMean     = NULL;
  double* clusterSigma    = NULL;
  double* sigma           = NULL;
  double* revSigma        = NULL;

  switch(mode)
  {
    case SMF__DEGLITCH_CORE:
      /* COMPUTE MEAN */
      start = -dsHalfLength + zpdIndex + 1;
      end = dsHalfLength + zpdIndex;
      for(i = start; i < end; i++)
      {
        mean += interferogram[i];
      }
      mean /= (end - start);

      /* COMPUTE CUTOFF AMPLITUDE */
      start = -(coreClusterSize >> 1) + zpdIndex + 1;
      end 	= (coreClusterSize >> 1) + zpdIndex;
      for(i = start; i < end; i++)
      {
        if(interferogram[i] > max) 
        { 
          max = interferogram[i]; 
        }
        if(interferogram[i] < min) 
        { 
          min = interferogram[i]; 
        }
      }
      cutoffAmplitude = 0.75 * (max - min);

      /* COMPUTE STANDARD DEVIATION */
      numCluster  	= dsHalfLength / coreClusterSize + 1;
      sigma    		= (double*) astMalloc(numCluster * sizeof(double));
      revSigma 		= (double*) astMalloc(numCluster * sizeof(double));
      for(i = 0; i < numCluster; i++)
      {
        tForward 	= 0.0;
        tReverse	= 0.0;
        start 		= (i - 1) * coreClusterSize;
        end 		= i * coreClusterSize;
        for(j = start; j < end; j++)
        {
          tForward += (interferogram[zpdIndex + j] - mean) * (interferogram[zpdIndex + j] - mean);
          tReverse += (interferogram[zpdIndex - j] - mean) * (interferogram[zpdIndex - j] - mean);
        }
        sigma[i] 	= sqrt(tForward / coreClusterSize);
        revSigma[i] = sqrt(tReverse / coreClusterSize);
      }

      maxSigma = (sigma[0] > sigma[1]) ? sigma[0] : sigma[1];

      /* CORE LENGTH */
      coreLength = (numCluster - 1) * coreClusterSize;
      for(i = 2; i < numCluster; i++)
      {
        if(sigma[i] < 0.1 * maxSigma)
        {
          coreLength = (i - 1) * coreClusterSize;
          break;
        }
      }

      /* REVERSE CORE LENGTH */
      revCoreLength = (numCluster - 1) * coreClusterSize;
      for(i = 2; i < numCluster; i++)
      {
        if(revSigma[i] < 0.1 * maxSigma)
        {
          revCoreLength = (i - 1) * coreClusterSize;
          break;
        }
      }

      /* COMPUTE THE CUTOFF FREQUENCY */
      cutoffFreq = (coreLength < revCoreLength) ? (10.0 / coreLength) : (10.0 / revCoreLength);

      start = -dsHalfLength + zpdIndex + 1;
      end 	= dsHalfLength + zpdIndex;
      for(i = start; i < end; i++)
      {
        tmp = abs((i - zpdIndex) * cutoffFreq);
        y = (tmp < (0.5 * PI)) ?
        		((tmp > threshold) ? cutoffAmplitude * sin(tmp) / tmp : cutoffAmplitude) :
        		cutoffAmplitude / tmp;

        error = interferogram[i] - mean;
        if(abs(error) > (2.0 * y))
        {
        	interferogram[i] = (error > 0) ? mean + y : mean - y;
        }
      }

      astFree(sigma);
      astFree(revSigma);
      break;
    case SMF__DEGLITCH_TAIL:
      tailDeglitchStart = dsHalfLength + zpdIndex;
      numCluster = (size - tailDeglitchStart) / tailClusterSize;

      clusterMean  = (double*) astMalloc(numCluster * sizeof(double));
      clusterSigma = (double*) astMalloc(numCluster * sizeof(double));

      for(i = 0; i < numCluster; i++)
      {
        start = tailDeglitchStart + i * tailClusterSize;
        end   = start + tailClusterSize;

        /* CLUSTER MEAN */
        clusterMean[i] = 0;
        for(j = start; j < end; j++)
        {
        	clusterMean[i] += interferogram[j];
        }
        clusterMean[i] /= tailClusterSize;

        /* CLUSTER STANDARD DEVIATION, SIGMA */
        clusterSigma[i] = 0.0;
        for(j = start; j < end; j++)
        {
          tmp = (interferogram[j] - clusterMean[i]);
          clusterSigma[i] += tmp * tmp;
        }
        clusterSigma[i] = sqrt(clusterSigma[i] / tailClusterSize);
      }
      /* SORT CLUSTER STANDARD DEVIATIONS IN ASCENDING ORDER */
      fts2_arrayquicksort(clusterSigma, numCluster, 0, numCluster - 1, 1);

      avgSigma = 0;
      start = 0;
      end = numCluster * tailCutoffStdDevPercent;
      for(i = start; i < end; i++)
      {
        avgSigma += clusterSigma[i];
      }
      avgSigma /= (end - start);

      cutoffSigma = avgSigma * tailCutoffStdDevMultiplier;
      start = 0;
      mean = 0;
      for(i = 0; i < numCluster; i++)
      {
        mean = clusterMean[i];
        start = tailDeglitchStart + i * tailClusterSize;
        end = start + tailClusterSize;
        for(j = start; j < end; j++)
        {
          if(interferogram[j] > (mean + cutoffSigma))
          {
            interferogram[j] = mean + cutoffSigma;
          }
          if(interferogram[j] < (mean - cutoffSigma))
          {
            interferogram[j] = mean - cutoffSigma;
          }
        }
      }

      start = tailDeglitchStart + numCluster * tailClusterSize;
      end = size;
      if(start < end)
      {
        mean = 0.0;
        for(j = start; j < end; j++)
        {
          mean += interferogram[j];
        }
        mean /= (end - start);

        for(j= start; j < end; j++)
        {
          if(interferogram[j] > (mean + cutoffSigma))
          {
            interferogram[j] = mean + cutoffSigma;
          }
          if(interferogram[j] < (mean - cutoffSigma))
          {
            interferogram[j] = mean - cutoffSigma;
          }
        }
      }
      astFree(clusterMean);
      astFree(clusterSigma);
      break;
    default: /* DEFAULT IS TO DEGLITCH ALL */
        fts2_deglitch( 
            interferogram,
            size,
            coreClusterSize,
            tailClusterSize,
            tailCutoffStdDevPercent,
            tailCutoffStdDevMultiplier,
            zpdIndex,
            dsHalfLength,
            SMF__DEGLITCH_CORE,
            threshold);

        fts2_deglitch( 
            interferogram,
            size,
            coreClusterSize,
            tailClusterSize,
            tailCutoffStdDevPercent,
            tailCutoffStdDevMultiplier,
            zpdIndex,
            dsHalfLength,
            SMF__DEGLITCH_TAIL,
            threshold);
        break;
  }
}