// 数据渲染 将程序的数据嵌入到网页中
// "key1":"小明"
// "key2":"小张"
// 数据字典

// 网页内容<p>{{key}}</p>

#include<iostream>
#include<string>
#include<ctemplate/template.h>
int main(){
    
    
    //形成数据字典
    //unordered_map<>test;
    std::string value = "11111test";
    ctemplate::TemplateDictionary root("test");
    root.SetValue("key", value);
    //获取被渲染的网页对象
    std::string in_html = "./test.html";
    ctemplate::Template *tpl = ctemplate::Template::GetTemplate(in_html,ctemplate::DO_NOT_STRIP );
    //将字典数据添加到网页中
    std::string out_html;
    tpl->Expand(&out_html, &root);

    //完成渲染
    std::cout<<out_html<<std::endl;

}