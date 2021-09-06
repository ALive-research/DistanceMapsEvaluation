#include <string>
#include <vtkGenericDataObjectReader.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkXMLPolyDataWriter.h>

int main(int argc, char* argv[])
{

  // Ensure a filename was specified
  if (argc != 3)
  {
    std::cerr << "Usage: " << argv[0] << " <input.vtk> <output.vtp>" << endl;
    return EXIT_FAILURE;
  }

  // Get the filename from the command line
  std::string inputFilename = argv[1];
  std::string outputFilename = argv[2];

  // Get all data from the file
  vtkNew<vtkGenericDataObjectReader> reader;
  reader->SetFileName(inputFilename.c_str());
  reader->Update();

  // All of the standard data types can be checked and obtained like this:
  if (reader->IsFilePolyData())
  {
    std::cout << "output is polydata," << std::endl;
    auto output = reader->GetPolyDataOutput();
    std::cout << "   output has " << output->GetNumberOfPoints() << " points."
              << std::endl;

    vtkNew<vtkXMLPolyDataWriter> polyDataWriter;
    polyDataWriter->SetInputData(output);
    polyDataWriter->SetFileName(outputFilename.c_str());
    polyDataWriter->Update();

  }

  return EXIT_SUCCESS;
}
