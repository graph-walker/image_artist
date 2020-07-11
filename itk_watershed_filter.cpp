#include <iostream>

#include "itkVectorGradientAnisotropicDiffusionImageFilter.h"
#include "itkVectorGradientMagnitudeImageFilter.h"
#include "itkWatershedImageFilter.h"
#include "itkImageFileReader.h"

#include "itkImageFileWriter.h"
#include "itkCastImageFilter.h"
#include "itkScalarToRGBPixelFunctor.h"
#include "itkPNGImageIOFactory.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkAdaptiveHistogramEqualizationImageFilter.h"

// adapted from itk example (itk.org)
namespace ia {
    const char* watershed_filter(const char* filename, double high_val, double low_val)
    {
        // handle pngs
        itk::PNGImageIOFactory::RegisterOneFactory();

        // defs
        using RGBPixelType = itk::RGBPixel<unsigned char>;
        using RGBImageType = itk::Image<RGBPixelType, 2>;
        using VectorPixelType = itk::Vector<float, 3>;
        using VectorImageType = itk::Image<VectorPixelType, 2>;
        using LabeledImageType = itk::Image<itk::IdentifierType, 2>;
        using ScalarImageType = itk::Image<float, 2>;

        using FileReaderType = itk::ImageFileReader<RGBImageType>;
        using CastFilterType = itk::CastImageFilter<RGBImageType, VectorImageType>;
        using DiffusionFilterType =
            itk::VectorGradientAnisotropicDiffusionImageFilter<VectorImageType,
            VectorImageType>;
        using GradientMagnitudeFilterType =
            itk::VectorGradientMagnitudeImageFilter<VectorImageType>;
        using WatershedFilterType = itk::WatershedImageFilter<ScalarImageType>;
        using FileWriterType = itk::ImageFileWriter<RGBImageType>;

        FileReaderType::Pointer reader = FileReaderType::New();
        reader->SetFileName(filename);

        CastFilterType::Pointer caster = CastFilterType::New();


        DiffusionFilterType::Pointer diffusion = DiffusionFilterType::New();
        diffusion->SetNumberOfIterations(70);
        diffusion->SetConductanceParameter(.8);
        diffusion->SetTimeStep(0.125);

        GradientMagnitudeFilterType::Pointer gradient =
            GradientMagnitudeFilterType::New();
        gradient->SetUsePrincipleComponents(0);

        using GaussFilterType = itk::DiscreteGaussianImageFilter<VectorImageType, VectorImageType>;
        GaussFilterType::Pointer GaussFilter = GaussFilterType::New();
        const double gauss_variance = .5;
        const unsigned int kernel_width = .001;
        GaussFilter->SetVariance(gauss_variance);
        GaussFilter->SetMaximumKernelWidth(kernel_width);

        using hist_type = itk::Image<unsigned char, 2>;
        using adaptive_histogram_type = itk::AdaptiveHistogramEqualizationImageFilter<hist_type>;
        adaptive_histogram_type::Pointer hist_filter = adaptive_histogram_type::New();

        float alpha = .1;
        hist_filter->SetAlpha(alpha);
        float beta = .05;
        hist_filter->SetBeta(beta);
        int radius_size = 1;
        adaptive_histogram_type::ImageSizeType radius;
        radius.Fill(radius_size);
        hist_filter->SetRadius(radius);

        WatershedFilterType::Pointer watershed = WatershedFilterType::New();
        watershed->SetLevel(high_val);
        watershed->SetThreshold(low_val);

        using ColormapFunctorType =
            itk::Functor::ScalarToRGBPixelFunctor<unsigned long>;
        using ColormapFilterType =
            itk::UnaryFunctorImageFilter<LabeledImageType,
            RGBImageType,
            ColormapFunctorType>;
        ColormapFilterType::Pointer colormapper = ColormapFilterType::New();

        FileWriterType::Pointer writer = FileWriterType::New();
        writer->SetFileName("watershed_result.png");

        auto p = diffusion->GetOutput();

        caster->SetInput(reader->GetOutput());
        diffusion->SetInput(caster->GetOutput());
        GaussFilter->SetInput(diffusion->GetOutput());


        using OutputPixelType = unsigned char;
        using input_type = itk::Image<VectorPixelType, 2>;
        using output_type = itk::Image<itk::RGBPixel<unsigned char>, 2U>;
        using CastingFilterType1 =
            itk::CastImageFilter<input_type, output_type>;
        CastingFilterType1::Pointer rs = CastingFilterType1::New();

        gradient->SetInput(diffusion->GetOutput());
        watershed->SetInput(gradient->GetOutput());
        colormapper->SetInput(watershed->GetOutput());
        writer->SetInput(colormapper->GetOutput());

        try
        {
            writer->Update();
        }
        catch (itk::ExceptionObject & e)
        {
            std::cerr << e << std::endl;

        }

        return "watershed_result.png";
    }
}