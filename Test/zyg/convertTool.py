import os
import re

# 获取当前文件夹路径
folder_path = os.getcwd()

# 遍历当前文件夹下的所有文件
for file_name in os.listdir(folder_path):
    if file_name.endswith(".txt"):
        file_path = os.path.join(folder_path, file_name)
        
        # 读取文件内容
        with open(file_path, 'r', encoding='gbk', errors='ignore') as file:
            content = file.read()
        
        # 使用正则表达式进行替换
        new_content = re.sub(r'while\s*\((.*?)\)', r'for(; \1 ;)', content)
        
        # 将替换后的内容写入文件
        with open(file_path, 'w') as file:
            file.write(new_content)
        
        print(f"文件 {file_name} 处理完成。")