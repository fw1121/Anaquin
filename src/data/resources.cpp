
#include <string>
#include <algorithm>

/*
 * Manuals
 */

#include "resources/manual.txt"
#include "resources/RnaAlign.txt"
#include "resources/RnaSubsample.txt"
#include "resources/RnaAssembly.txt"
#include "resources/RnaExpression.txt"
#include "resources/RnaFoldChange.txt"
#include "resources/VarAlign.txt"
#include "resources/VarDiscover.txt"
#include "resources/VarSubsample.txt"

/*
 * Scripts
 */

#include "resources/reports.py"

#include "resources/plotFold.R"
#include "resources/plotScatter.R"
#include "resources/plotSensitivity.R"

/*
 * Transcriptome Resources
 */

#include "resources/plotTROC.R"
#include "resources/plotTLODR.R"

#include "resources/ARN020_v001.gtf"
#include "resources/MRN027_v001.csv"
#include "resources/MRN028_v001.csv"
#include "resources/MRN029_v001.csv"
#include "resources/MRN030_v001.csv"

/*
 * Variant Resources
 */

#include "resources/plotVLOD.R"
#include "resources/plotVROC1.R"

#include "resources/sampled.bed"
#include "resources/AVA009_v001.vcf"
#include "resources/MVA011_v001.csv"
#include "resources/MVA012_v001.csv"

typedef std::string Scripts;

#define ToString(x) std::string(reinterpret_cast<char*>(x))

Scripts ReportScript()
{
    return ToString(scripts_reports_py);
}

Scripts Manual()
{
    return ToString(data_manual_txt);
}

Scripts PlotScatter()
{
    return ToString(src_r_plotScatter_R);
}

Scripts PlotFold()
{
    return ToString(src_r_plotFold_R);
}

Scripts PlotSensitivity()
{
    return ToString(src_r_plotSensitivity_R);
}

/*
 * Manuals
 */

Scripts RnaSubsample()
{
    return ToString(data_manuals_RnaSubsample_txt);
}

Scripts RnaAlign()
{
    return ToString(data_manuals_RnaAlign_txt);
}

Scripts RnaAssembly()
{
    return ToString(data_manuals_RnaAssembly_txt);
}

Scripts RnaExpression()
{
    return ToString(data_manuals_RnaExpression_txt);
}

Scripts RnaFoldChange()
{
    return ToString(data_manuals_RnaFoldChange_txt);
}

Scripts VarAlign()
{
    return ToString(data_manuals_VarAlign_txt);
}

Scripts VarSubsample()
{
    return ToString(data_manuals_VarSubsample_txt);
}

Scripts VarDiscover()
{
    return ToString(data_manuals_VarDiscover_txt);
}

/*
 * Transcriptome Resources
 */

Scripts PlotTROC()
{
    return ToString(src_r_plotTROC_R);
}

Scripts PlotTLODR()
{
    return ToString(src_r_plotTLODR_R);
}

Scripts RnaStandGTF()
{
    return ToString(data_RnaQuin_ARN020_v001_gtf);
}

Scripts RnaDataMixA()
{
    return ToString(data_RnaQuin_MRN027_v001_csv);
}

Scripts RnaDataMixB()
{
    return ToString(data_RnaQuin_MRN028_v001_csv);
}

Scripts RnaDataMixF()
{
    return ToString(data_RnaQuin_MRN030_v001_csv);
}

Scripts RnaDataMixAB()
{
    return ToString(data_RnaQuin_MRN029_v001_csv);
}

/*
 * Variant Resources
 */

Scripts PlotVLOD()
{
    return ToString(src_r_plotVLOD_R);
}

Scripts PlotVROC()
{
    return ToString(src_r_plotVROC1_R);
}

Scripts VarDataMixA()
{
    return ToString(data_VarQuin_MVA011_v001_csv);
}

Scripts VarDataMixF()
{
    return ToString(data_VarQuin_MVA012_v001_csv);
}

Scripts VarDataVCF()
{
    return ToString(data_VarQuin_AVA009_v001_vcf);
}

Scripts VarDataBed()
{
    return ToString(data_VarQuin_sampled_bed);
}