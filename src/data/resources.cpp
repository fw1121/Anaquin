#include <string>
#include <algorithm>

/*
 * Manuals
 */

#include "resources/VarCopy.txt"
#include "resources/VarDetect.txt"
#include "resources/anaquin.txt"
#include "resources/VarFlip.txt"
#include "resources/VarTrim.txt"
#include "resources/VarAlign.txt"
#include "resources/VarKAbund.txt"
#include "resources/VarSomatic.txt"
#include "resources/VarConjoint.txt"
#include "resources/VarStructure.txt"
#include "resources/VarSubsample.txt"

#include "resources/RnaAlign.txt"
#include "resources/RnaAssembly.txt"
#include "resources/RnaSubsample.txt"
#include "resources/RnaExpression.txt"
#include "resources/RnaFoldChange.txt"

#include "resources/MetaAlign.txt"
#include "resources/MetaSubsample.txt"
#include "resources/MetaAssembly.txt"
#include "resources/MetaAbund.txt"
#include "resources/MetaFoldChange.txt"

#include "resources/plotCNV.R"
#include "resources/plotFold.R"
#include "resources/plotTROC.R"
#include "resources/plotVLODR.R"
#include "resources/plotVGROC.R"
#include "resources/plotVCROC.R"
#include "resources/plotTLODR.R"
#include "resources/plotAllele.R"
#include "resources/plotLinear.R"
#include "resources/plotKAllele.R"
#include "resources/plotConjoint.R"
#include "resources/plotLogistic.R"

/*
 * RnaQuin Resources
 */

#include "resources/A.R.1.gtf"
#include "resources/MRN027_v001.csv"
#include "resources/MRN029_v001.csv"

/*
 * VarQuin Resources
 */

#include "resources/AVA033_v001.bed"

typedef std::string Scripts;

#define ToString(x) std::string(reinterpret_cast<char*>(x))

Scripts Manual()
{
    return ToString(data_manuals_anaquin_txt);
}

Scripts PlotFold()
{
    return ToString(src_r_plotFold_R);
}

Scripts PlotCNV()      { return ToString(src_r_plotCNV_R);      }
Scripts PlotLinear()   { return ToString(src_r_plotLinear_R);   }
Scripts PlotConjoint() { return ToString(src_r_plotConjoint_R); }
Scripts PlotAllele()   { return ToString(src_r_plotAllele_R);   }
Scripts PlotKAllele()  { return ToString(src_r_plotKAllele_R);  }
Scripts PlotLogistic() { return ToString(src_r_plotLogistic_R); }

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

Scripts VarTrim()      { return ToString(data_manuals_VarTrim_txt);      }
Scripts VarFlip()      { return ToString(data_manuals_VarFlip_txt);      }
Scripts VarCopy()      { return ToString(data_manuals_VarCopy_txt);      }
Scripts VarAlign()     { return ToString(data_manuals_VarAlign_txt);     }
Scripts VarKAbund()    { return ToString(data_manuals_VarKAbund_txt);    }
Scripts VarSample()    { return ToString(data_manuals_VarSubsample_txt); }
Scripts VarDetect()    { return ToString(data_manuals_VarDetect_txt);    }
Scripts VarSomatic()   { return ToString(data_manuals_VarSomatic_txt);   }
Scripts VarConjoint()  { return ToString(data_manuals_VarConjoint_txt);  }
Scripts VarStructure() { return ToString(data_manuals_VarStructure_txt); }

Scripts MetaAlign()
{
    return ToString(data_manuals_MetaAlign_txt);
}

Scripts MetaSubsample()
{
    return ToString(data_manuals_MetaSubsample_txt);
}

Scripts MetaAssembly()
{
    return ToString(data_manuals_MetaAssembly_txt);
}

Scripts MetaAbund()
{
    return ToString(data_manuals_MetaAbund_txt);
}

Scripts MetaFoldChange()
{
    return ToString(data_manuals_MetaFoldChange_txt);
}

/*
 * RnaSeq Resources
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
    return ToString(data_RnaQuin_A_R_1_gtf);
}

Scripts RnaDataMixA()
{
    return ToString(data_RnaQuin_MRN027_v001_csv);
}

Scripts RnaDataMixAB()
{
    return ToString(data_RnaQuin_MRN029_v001_csv);
}

/*
 * Variant Resources
 */

Scripts PlotVLODR()
{
    return ToString(src_r_plotVLODR_R);
}

Scripts PlotVGROC()
{
    return ToString(src_r_plotVGROC_R);
}

Scripts PlotVCROC()
{
    return ToString(src_r_plotVCROC_R);
}

Scripts AVA033Bed()
{
    return ToString(data_VarQuin_AVA033_v001_bed);
}
