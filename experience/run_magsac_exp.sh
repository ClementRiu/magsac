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

printf "\nReading data folder: ${expFolder}\n"

printf "\n\n---------- Running experiment. ----------\n\n\n\n"

expBegin=1
if [ "${expModel}" == "0" ];then
    folderName="hom"
    modelPrefix="hom"
    inFolderGlob="${inFolderGlob}homog/"
    expEnd=10
fi
if [ "${expModel}" == "1" ];then
    folderName="fun"
    modelPrefix="fun"
    inFolderGlob="${inFolderGlob}fundmatrix/"
    expEnd=11
fi
if [ "${expModel}" == "2" ];then
    folderName="ess"
    modelPrefix="ess"
    inFolderGlob="${inFolderGlob}essential/"
    expEnd=6
fi

inFolderGlob="/home/riuclement/Documents/USAC/data/essential/"

expStdNoiseMin=0.0
expStdNoiseMax=3.0
expStdNoiseStep=0.1

expStdNoiseCounterMin=0 #Used for the while loop.
expStdNoiseCounterMax=30 #Used for the while loop. = (expStdNoiseMax - expStdNoiseMin) / expStdNoiseStep

expStdNoise=${expStdNoiseMin}
expStdNoiseCounter=${expStdNoiseCounterMin} #Used for the while loop.

expOutlierRatioMin=0.1
expOutlierRatioMax=0.9
expOutlierRatioStep=0.1

expOutlierRatioCounterMin=1 #Used for the while loop.
expOutlierRatioCounterMax=9 #Used for the while loop. = (expOutlierRatioMax - expOutlierRatioMin) / expOutlierRatioStep

expOutlierRatio=${expOutlierRatioMin}
expOutlierRatioCounter=${expOutlierRatioCounterMin} #Used for the while loop.

outExpPathU="${expFolder}/uniform"
expCounterU=${expBegin}

while [ "$expCounterU" -ge "$expBegin" ] && [ "$expCounterU" -le "$expEnd" ]
do
    while [ "${expOutlierRatioCounter}" -ge "${expOutlierRatioCounterMin}" ] && [ "${expOutlierRatioCounter}" -le "${expOutlierRatioCounterMax}" ]
    do
        while [ "$expStdNoiseCounter" -ge "$expStdNoiseCounterMin" ] && [ "$expStdNoiseCounter" -le "$expStdNoiseCounterMax" ]
        do
        #Gaussian Noise:

            expArgs="-i ${expIterMax} -u ${expModel} -m ${expMagsacPartition} -l ${expTime} -s ${expMaxSigma} -c ${expConfidence} -r ${expRefThreshold} -e ${expNGen} -n ${expNRun}"


            if [ "${expMagsacPP}" -eq "1" ]; then
                expArgs="${expArgs} -p"
            fi
            if [ "${expVerbose}" -eq "1" ]; then
                expArgs="${expArgs} -v"
            fi
            if [ "${expSeed}" -ge "0" ]; then
                expArgs="${expArgs} -t ${expSeed}"
            fi

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

            ~/Documents/magsac/bin/RunExperience $expArgs ${inInliers} ${inOutliers} ${outInfo} ${outLabels} ${outPosInl} ${outWeights} ${outInliers} ${outErrors} ${outErrorsAll} ${inCalib}

            printf "Done.\n\n"

            expStdNoise="$(echo "${expStdNoise} + ${expStdNoiseStep}" | bc)"
            expStdNoiseCounter=$((${expStdNoiseCounter}+1))

        done # End of the std noise loop.

        expOutlierRatio="$(echo "${expOutlierRatio} + ${expOutlierRatioStep}" | bc)"
        expOutlierRatioCounter=$((${expOutlierRatioCounter}+1))

        expStdNoise=${expStdNoiseMin}
        expStdNoiseCounter=${expStdNoiseCounterMin}

        expStdNoise=${expStdNoiseMin}
        expStdNoiseCounter=0 #Used for the while loop.

    done # End of the outlier ratio loop.
    expCounter=$((${expCounter}+1))

    expOutlierRatio=${expOutlierRatioMin}
    expOutlierRatioCounter=${expOutlierRatioCounterMin}
done # End of the folder loop.

expCounterU=${expBegin}
while [ "${expCounterU}" -ge "${expBegin}" ] && [ "${expCounterU}" -le "${expEnd}" ]
do
    while [ "${expStdNoiseCounter}" -ge "${expStdNoiseCounterMin}" ] && [ "${expStdNoiseCounter}" -le "${expStdNoiseCounterMax}" ]
    do

        expArgs="-i ${expIterMax} -u ${expModel} -m ${expMagsacPartition} -l ${expTime} -s ${expMaxSigma} -c ${expConfidence} -r ${expRefThreshold} -e ${expNGen} -n ${expNRun} -o"


        if [ "${expMagsacPP}" -eq "1" ]; then
            expArgs="${expArgs} -p"
        fi
        if [ "${expVerbose}" -eq "1" ]; then
            expArgs="${expArgs} -v"
        fi
        if [ "${expSeed}" -ge "0" ]; then
            expArgs="${expArgs} -t ${expSeed}"
        fi

        inInliers="${outExpPathU}/${folderName}_${expCounterU}_std${expStdNoise}_ratio0.0_NoisyIn.txt"
        inOutliers="${outExpPathU}/${folderName}_${expCounterU}_std${expStdNoise}_ratio0.0_Outliers.txt"

        outInfo="${outExpPathU}/${folderName}_${expCounterU}_std${expStdNoise}_ratio0.0_magsacOut.txt"
        outLabels="${outExpPathU}/${folderName}_${expCounterU}_std${expStdNoise}_ratio0.0_magsacLabels.txt"
        outPosInl="${outExpPathU}/${folderName}_${expCounterU}_std${expStdNoise}_ratio0.0_magsacPosInl.txt"
        outWeights="${outExpPathU}/${folderName}_${expCounterU}_std${expStdNoise}_ratio0.0_magsacWeights.txt"
        outInliers="${outExpPathU}/${folderName}_${expCounterU}_std${expStdNoise}_ratio0.0_magsacInliers.txt"
        outErrors="${outExpPathU}/${folderName}_${expCounterU}_std${expStdNoise}_ratio0.0_magsacErrors.txt"
        outErrorsAll="${outExpPathU}/${folderName}_${expCounterU}_std${expStdNoise}_ratio0.0_magsacErrorsAll.txt"

        printf "Reading ${outExpPathU}/${folderName}_${expCounterU}_std${expStdNoise}_ratio0.0_... ."

        ~/Documents/magsac/bin/RunExperience $expArgs ${inInliers} ${inOutliers} ${outInfo} ${outLabels} ${outPosInl} ${outWeights} ${outInliers} ${outErrors} ${outErrorsAll}

        printf "Done.\n\n"

        expStdNoise="$(echo "${expStdNoise} + ${expStdNoiseStep}" | bc)"
        expStdNoiseCounter=$((${expStdNoiseCounter}+1))

    done # End of the std noise loop.


    expStdNoise=${expStdNoiseMin}
    expStdNoiseCounter=${expStdNoiseCounterMin}

    expCounterU=$((${expCounterU}+1))

done # End of the folder loop.
