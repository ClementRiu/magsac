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
    printf "\nInput folder requiered:\n"
    exit 1
fi

if [ "${expModel}" == "0" ];then
    folderName="hom"
else
    folderName="fun"
fi

printf "\nReading data folder: ${expFolder}\n"

printf "\n\n---------- Running experiment. ----------\n\n\n\n"

expCounter=1
expBegin=1

expStdNoiseMin=0.0
expStdNoiseMax=3.0
expStdNoiseStep=0.1

expStdNoiseCounterMin=0 #Used for the while loop.
expStdNoiseCounterMax=30 #Used for the while loop. = (expStdNoiseMax - expStdNoiseMin) / expStdNoiseStep

expStdNoise=${expStdNoiseMin}
expStdNoiseCounter=${expStdNoiseCounterMin} #Used for the while loop.

expOutlierRatioMin=.1
expOutlierRatioMax=.9
expOutlierRatioStep=0.1

expOutlierRatioCounterMin=1 #Used for the while loop.
expOutlierRatioCounterMax=9 #Used for the while loop. = (expOutlierRatioMax - expOutlierRatioMin) / expOutlierRatioStep

expOutlierRatio=${expOutlierRatioMin}
expOutlierRatioCounter=${expOutlierRatioCounterMin} #Used for the while loop.

outExpPathG="${expFolder}/gaussian"
outExpPathU="${expFolder}/uniform"

while [ "$expStdNoiseCounter" -ge "$expStdNoiseCounterMin" ] && [ "$expStdNoiseCounter" -le "$expStdNoiseCounterMax" ]
do
    while [ "$expOutlierRatioCounter" -ge "$expOutlierRatioCounterMin" ] && [ "$expOutlierRatioCounter" -le "$expOutlierRatioCounterMax" ]
    do
        #Gaussian Noise:

        expArgs="-i ${expIterMax} -u ${expModel} -m ${expMagsacPartition} -l ${expTime} -s ${expMaxSigma} -c ${expConfidence} -r ${expRefThreshold} -e ${expNGen} -n ${expNRun}"


        if [ "${expMagsacPP}" -eq "1" ]; then
            expArgs="${expArgs} -p"
        fi
        if [ "${expOutlierRatioCounter}" -eq "0" ]; then
            expArgs="${expArgs} -o"
        fi
        if [ "${expVerbose}" -eq "1" ]; then
            expArgs="${expArgs} -v"
        fi
        if [ "${expSeed}" -ge "0" ]; then
            expArgs="${expArgs} -t ${expSeed}"
        fi

        # expCounterG=${expBegin}
        #
        # while [ "$expCounterG" -ge "$expBegin" ] && [ "$expCounterG" -le "$expEnd" ]
        # do
        #     inInliers="${outExpPathG}/${folderName}_${expCounterG}_std${expStdNoise}_ratio${expOutlierRatio}_NoisyIn.txt"
        #     inOutliers="${outExpPathG}/${folderName}_${expCounterG}_std${expStdNoise}_ratio${expOutlierRatio}_Outliers.txt"
        #
        #     outInfo="${outExpPathG}/${folderName}_${expCounterG}_std${expStdNoise}_ratio${expOutlierRatio}_magsacOut.txt"
        #     outLabels="${outExpPathG}/${folderName}_${expCounterG}_std${expStdNoise}_ratio${expOutlierRatio}_magsacLabels.txt"
        #     outPosInl="${outExpPathG}/${folderName}_${expCounterG}_std${expStdNoise}_ratio${expOutlierRatio}_magsacPosInl.txt"
        #     outWeights="${outExpPathG}/${folderName}_${expCounterG}_std${expStdNoise}_ratio${expOutlierRatio}_magsacWeights.txt"
        #     outInliers="${outExpPathG}/${folderName}_${expCounterG}_std${expStdNoise}_ratio${expOutlierRatio}_magsacInliers.txt"
        #     outErrors="${outExpPathG}/${folderName}_${expCounterG}_std${expStdNoise}_ratio${expOutlierRatio}_magsacErrors.txt"
        #     outErrorsAll="${outExpPathG}/${folderName}_${expCounterG}_std${expStdNoise}_ratio${expOutlierRatio}_magsacErrorsAll.txt"
        #
        #     printf "Reading ${outExpPathG}/${folderName}_${expCounterG}_std${expStdNoise}_ratio${expOutlierRatio}_... ."
        #
        #     ~/Documents/these/magsac/bin/RunExperience $expArgs ${inInliers} ${inOutliers} ${outInfo} ${outLabels} ${outPosInl} ${outWeights} ${outInliers} ${outErrors} ${outErrorsAll}
        #
        #     printf "Done.\n\n"
        #
        #     expCounterG=$(($expCounterG+1))
        # done # End of the folder loop for Gaussian noise

        #Uniform Noise:

        # expArgs="${expArgs}"

        expCounterU=${expBegin}

        while [ "$expCounterU" -ge "$expBegin" ] && [ "$expCounterU" -le "$expEnd" ]
        do
            inInliers="${outExpPathU}/${folderName}_${expCounterU}_std${expStdNoise}_ratio${expOutlierRatio}_NoisyIn.txt"
            inOutliers="${outExpPathU}/${folderName}_${expCounterU}_std${expStdNoise}_ratio${expOutlierRatio}_Outliers.txt"

            outInfo="${outExpPathU}/${folderName}_${expCounterU}_std${expStdNoise}_ratio${expOutlierRatio}_magsacOut.txt"
            outLabels="${outExpPathU}/${folderName}_${expCounterU}_std${expStdNoise}_ratio${expOutlierRatio}_magsacLabels.txt"
            outPosInl="${outExpPathU}/${folderName}_${expCounterU}_std${expStdNoise}_ratio${expOutlierRatio}_magsacPosInl.txt"
            outWeights="${outExpPathU}/${folderName}_${expCounterU}_std${expStdNoise}_ratio${expOutlierRatio}_magsacWeights.txt"
            outInliers="${outExpPathU}/${folderName}_${expCounterU}_std${expStdNoise}_ratio${expOutlierRatio}_magsacInliers.txt"
            outErrors="${outExpPathU}/${folderName}_${expCounterU}_std${expStdNoise}_ratio${expOutlierRatio}_magsacErrors.txt"
            outErrorsAll="${outExpPathU}/${folderName}_${expCounterU}_std${expStdNoise}_ratio${expOutlierRatio}_magsacErrorsAll.txt"

            printf "Reading ${outExpPathU}/${folderName}_${expCounterU}_std${expStdNoise}_ratio${expOutlierRatio}_... ."

            ~/Documents/these/magsac/bin/RunExperience $expArgs ${inInliers} ${inOutliers} ${outInfo} ${outLabels} ${outPosInl} ${outWeights} ${outInliers} ${outErrors} ${outErrorsAll}

            printf "Done.\n\n"

            expCounterU=$(($expCounterU+1))
        done # End of the folder loop for Uniform noise

        expOutlierRatio="$(echo "$expOutlierRatio + $expOutlierRatioStep" | bc)"
        expOutlierRatioCounter=$(($expOutlierRatioCounter+1))

    done # End of the outlier ratio loop.
    expStdNoise="$(echo "$expStdNoise + $expStdNoiseStep" | bc)"
    expStdNoiseCounter=$(($expStdNoiseCounter+1))

    expOutlierRatio=${expOutlierRatioMin}
    expOutlierRatioCounter=${expOutlierRatioCounterMin} #Used for the while loop.

done # End of the std noise loop.
