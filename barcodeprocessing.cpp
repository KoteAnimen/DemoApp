#include "barcodeprocessing.h"

#include <QDateTime>
#include <QDir>

#include <exception>
#include <stdio.h>


BarCodeProcessing::BarCodeProcessing()
{
    reader = new dynamsoft::dbr::CBarcodeReader();
    PublicRuntimeSettings runtimeSettings;

    int iLicMsg = -1;
    // Use the SDK offline
    iLicMsg = reader->InitLicense("t0069fQAAAGldGmzJgbEqF7BiWUjz13KgUNhGin8MOQP0zRIl9yeiA35ysiizj47TkgDigErc0F2ZHrrSwpHfvXKVK/WwItTl");
    char szErrorMsg[256];

    reader->InitRuntimeSettingsWithString("{\"ImageParameter\":{\"Name\":\"BestCoverage\","
                                      "\"DeblurLevel\":2,"
                                      "\"ExpectedBarcodesCount\":0,"
                                      "\"ScaleDownThreshold\":100000,"
                                      "\"LocalizationModes\":[{\"Mode\":\"LM_CONNECTED_BLOCKS\"}],"
                                      "\"GrayscaleTransformationModes\":[{\"Mode\":\"GTM_ORIGINAL\"}]}}",
                                          CM_OVERWRITE, szErrorMsg, 256);

    reader->GetRuntimeSettings(&runtimeSettings);
    runtimeSettings.barcodeFormatIds = BF_DATAMATRIX;
    runtimeSettings.barcodeFormatIds_2 = 0;
}

int BarCodeProcessing::getLastGoodFrame() const
{
    return lastGoodFrame;
}

void BarCodeProcessing::setLastGoodFrame(int value)
{
    lastGoodFrame = value;
}
int BarCodeProcessing::getFrameToSave() const
{
    return frameToSave;
}

void BarCodeProcessing::setFrameToSave(int value)
{
    frameToSave = value;
}
QString BarCodeProcessing::getFolderName() const
{
    return FolderName;
}

void BarCodeProcessing::setFolderName(const QString &value)
{
}
bool BarCodeProcessing::getQrFound() const
{
    return qrFound;
}

void BarCodeProcessing::setQrFound(bool value)
{
    qrFound = value;
}

void BarCodeProcessing::setNeedScale(bool value)
{
    needScale = value;
}

bool BarCodeProcessing::getEanFound() const
{
    return eanFound;
}

void BarCodeProcessing::setEanFound(bool value)
{
    eanFound = value;
}

int BarCodeProcessing::getMainThresh() const
{
    return MainThresh;
}

void BarCodeProcessing::setMainThresh(int value)
{
    MainThresh = value;

}

void BarCodeProcessing::writemat()
{


}

void BarCodeProcessing::UpdateDB()
{

}

void BarCodeProcessing::ProcessFrame(cv::Mat frame)
{    
    if(frame.rows*frame.cols>0)
    {
        // cv::resize(frame,frame,cv::Size(), 0.4, 0.4, cv::INTER_CUBIC);
        unsigned __int64 ullTimeBegin = 0;
        unsigned __int64 ullTimeEnd = 0;
        int width = frame.cols;
        int iRet;
        ullTimeBegin = GetTickCount();
        iRet  = reader->DecodeBuffer(frame.data, frame.cols, frame.rows, width, IPF_GRAYSCALED);
        ullTimeEnd = GetTickCount();
        emit ElapsedTime(ullTimeEnd-ullTimeBegin);

        TextResultArray *paryResult = NULL;
        reader->GetAllTextResults(&paryResult);
        cv::cvtColor(frame,frame,cv::COLOR_GRAY2RGB);
        for (int iIndex = 0; iIndex < paryResult->resultsCount; iIndex++)
        {
            QString res  =  QString(paryResult->results[iIndex]->barcodeText);
            PLocalizationResult lr;
            lr = paryResult->results[iIndex]->localizationResult;

            QString type = paryResult->results[iIndex]->barcodeFormatString;
            cv::Rect rec;
            if(!readCodes.contains(res))
            {
                emit CodeQr(res);
                qrFound = true;
                readCodes.append(res);                
                rec = cv::Rect(cv::Point(lr->x1,lr->y1),cv::Point(lr->x3,lr->y3));
                cv::rectangle(frame,rec,cv::Scalar(0,255,0),10);                
            }
            else
            {
                rec = cv::Rect(cv::Point(lr->x1,lr->y1),cv::Point(lr->x3,lr->y3));
                cv::rectangle(frame,rec,cv::Scalar(255,0,0),10);
            }            
        }
        readCodes.clear();
        emit ThreshMat(frame);
        emit NextFrame();

        dynamsoft::dbr::CBarcodeReader::FreeTextResults(&paryResult);
    }
}


