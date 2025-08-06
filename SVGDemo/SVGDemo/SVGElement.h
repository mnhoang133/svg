#ifndef SVGElement_h
#define SVGElement_h
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <regex>
#include <sstream>


class SVGElement 
{
protected:
    Gdiplus::Matrix transform;
public:
       virtual void render(Gdiplus::Graphics* graphics) = 0;

       virtual void setTransform(const std::wstring& transformStr) {
           std::wregex pattern(L"(translate|scale|rotate)\\s*\\(([^\\)]+)\\)");
           auto begin = std::wsregex_iterator(transformStr.begin(), transformStr.end(), pattern);
           auto end = std::wsregex_iterator();

           for (auto it = begin; it != end; ++it) {
               std::wstring cmd = (*it)[1].str();
               std::wstring args = (*it)[2].str();
               std::wstringstream ss(args);
               std::vector<float> values;
               float val;
               while (ss >> val) {
                   values.push_back(val);
                   if (ss.peek() == ',' || ss.peek() == ' ') ss.ignore();
               }

               if (cmd == L"translate" && (values.size() == 1 || values.size() == 2)) {
                   transform.Translate(values[0], values.size() == 2 ? values[1] : 0);
               }
               else if (cmd == L"scale" && (values.size() == 1 || values.size() == 2)) {
                   transform.Scale(values[0], values.size() == 2 ? values[1] : values[0]);
               }
               else if (cmd == L"rotate" && values.size() == 1) {
                   transform.Rotate(values[0]);
               }
           }
       }

       virtual const Gdiplus::Matrix& getTransform() const { return transform; }

       virtual void applyTransform(Gdiplus::Graphics* graphics) const {
           if (graphics) {
               graphics->MultiplyTransform(&transform); // `transform` là Matrix member của SVGElement
           }
       }


       virtual ~SVGElement() = default;
};
#endif 