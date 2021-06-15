#!/bin/bash

expFolder=${expFolder:--1} # Give a time folder here to set the input folder.

expVerbose=${expVerbose:-0}
expSeed=${expSeed:--1}

expIterMax=${expIterMax:-10000}
expModel=${expModel:-0}
expMagsacPP=${expMagsacPP:-0}
expMagsacPartition=${expMagsacPartition:-10}
expTime=${expTime:-2}
expMaxSigma=${expMaxSigma:-10}
expConfidence=${expConfidence:-0.99}
expRefThreshold=${expRefThreshold:-2.0}
expNGen=${expNGen:-5}
expNRun=${expNRun:-5}

expEnd=${expEnd:-10}

while [ $# -gt 0 ]; do

   if [[ $1 == *"--"* ]]; then
        param="${1/--/}"
        declare $param="$2"
   fi

  shift
done

#Generation of the paths and creation of the necessary folders:

if [ "${expFolder}" == "-1" ];then
    printf "\nInput folder requiered (expFolder):\n"
    exit 1
fi

if [ "${expModel}" == "0" ];then
    folderName="hom"
fi
if [ "${expModel}" == "1" ];then
    folderName="fun"
fi
if [ "${expModel}" == "2" ];then
    folderName="ess"
fi

inFolderGlob="/home/clementriu/Documents/these/USAC/data/essential/"


printf "\nReading data folder: ${expFolder}\n"

printf "\n\n---------- Running experiment. ----------\n\n\n\n"

expCounter=1
expBegin=1

expStdNoiseMin=0.0
expStdNoiseMax=3.0
expStdNoiseStep=0.2

expStdNoiseCounterMin=0 #Used for the while loop.
expStdNoiseCounterMax=15 #Used for the while loop. = (expStdNoiseMax - expStdNoiseMin) / expStdNoiseStep

expStdNoise=${expStdNoiseMin}
expStdNoiseCounter=${expStdNoiseCounterMin} #Used for the while loop.

expOutlierRatioMin=0.9
expOutlierRatioMax=0.0
expOutlierRatioStep=0.1

expOutlierRatioCounterMin=0 #Used for the while loop.
expOutlierRatioCounterMax=9 #Used for the while loop. = (expOutlierRatioMax - expOutlierRatioMin) / expOutlierRatioStep

expOutlierRatio=${expOutlierRatioMin}
expOutlierRatioCounter=${expOutlierRatioCounterMin} #Used for the while loop.

outExpPathU="${expFolder}/uniform"
expCounterU=${expBegin}

while [ "$expCounterU" -ge "$expBegin" ] && [ "$expCounterU" -le "$expEnd" ]
do
    while [ "$expOutlierRatioCounter" -ge "$expOutlierRatioCounterMin" ] && [ "$expOutlierRatioCounter" -le "$expOutlierRatioCounterMax" ]
    do
        while [ "$expStdNoiseCounter" -ge "$expStdNoiseCounterMin" ] && [ "$expStdNoiseCounter" -le "$expStdNoiseCounterMax" ]
        do
        #Gaussian Noise:

            expArgs="-i ${expIterMax} -u ${expModel} -m ${expMagsacPartition} -l ${expTime} -s ${expMaxSigma} -c ${expConfidence} -r ${expRefThreshold} -e ${expNGen} -n ${expNRun}"


            if [ "${expMagsacPP}" -eq "1" ]; then
                expArgs="${expArgs} -p"
            fi
            if [ "${expOutlierRatioCounter}" -eq "9" ]; then
                expArgs="${expArgs} -o"
            fi
            if [ "${expVerbose}" -eq "1" ]; then
                expArgs="${expArgs} -v"
            fi
            if [ "${expSeed}" -ge "0" ]; then
                expArgs="${expArgs} -t ${expSeed}"
            fi

            #Uniform Noise:

            # expArgs="${expArgs}"

            inInliers="${outExpPathU}/${folderName}_${expCounterU}_std${expStdNoise}_ratio${expOutlierRatio}_NoisyIn.txt"
            inOutliers="${outExpPathU}/${folderName}_${expCounterU}_std${expStdNoise}_ratio${expOutlierRatio}_Outliers.txt"

            inCalib=${inFolderGlob}test${expCounterU}/calib_matrices.txt

            outInfo="${outExpPathU}/${folderName}_${expCounterU}_std${expStdNoise}_ratio${expOutlierRatio}_magsacOut.txt"
            outLabels="${outExpPathU}/${folderName}_${expCounterU}_std${expStdNoise}_ratio${expOutlierRatio}_magsacLabels.txt"
            outPosInl="${outExpPathU}/${folderName}_${expCounterU}_std${expStdNoise}_ratio${expOutlierRatio}_magsacPosInl.txt"
            outWeights="${outExpPathU}/${folderName}_${expCounterU}_std${expStdNoise}_ratio${expOutlierRatio}_magsacWeights.txt"
            outInliers="${outExpPathU}/${folderName}_${expCounterU}_std${expStdNoise}_ratio${expOutlierRatio}_magsacInliers.txt"
            outErrors="${outExpPathU}/${folderName}_${expCounterU}_std${expStdNoise}_ratio${expOutlierRatio}_magsacErrors.txt"
            outErrorsAll="${outExpPathU}/${folderName}_${expCounterU}_std${expStdNoise}_ratio${expOutlierRatio}_magsacErrorsAll.txt"

            printf "Reading ${outExpPathU}/${folderName}_${expCounterU}_std${expStdNoise}_ratio${expOutlierRatio}_... ."

            ~/Documents/these/magsac/bin/RunExperience $expArgs ${inInliers} ${inOutliers} ${outInfo} ${outLabels} ${outPosInl} ${outWeights} ${outInliers} ${outErrors} ${outErrorsAll} ${inCalib}

            printf "Done.\n\n"
            expStdNoise="$(echo "$expStdNoise + $expStdNoiseStep" | bc)"
            expStdNoiseCounter=$(($expStdNoiseCounter+1))
        done # End of the folder loop for Uniform noise

        expOutlierRatio="$(echo "$expOutlierRatio - $expOutlierRatioStep" | bc)"
        expOutlierRatioCounter=$(($expOutlierRatioCounter+1))

        expStdNoise=${expStdNoiseMin}
        expStdNoiseCounter=0 #Used for the while loop.

    done # End of the outlier ratio loop.

    expOutlierRatio=${expOutlierRatioMin}
    expOutlierRatioCounter=${expOutlierRatioCounterMin} #Used for the while loop.
    expCounterU=$(($expCounterU+1))

done # End of the std noise loop.
