#!/usr/bin/python
# -*- coding: UTF-8 -*-

import smtplib, random
from email.mime.text import MIMEText
from email.utils import formataddr

arr = [0,1,2,3,4,5,6,7,8,9,'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'];

class mail:
    def __init__(self):
        self.pwd='uprituqxijwjdaed'
        self.sender='3457554915@qq.com'

    def get_code(self):
        res = ""
        for i in range(6):
            idx = random.randint(0, len(arr) - 1)
            res += str(arr[idx]);
        return res;

    def send(self, code, receiver):
        ret=True
        try:
            msg=MIMEText('验证码为：' + code,'plain','utf-8')
            msg['From']=formataddr(["Admin",self.sender])  # 括号里的对应发件人邮箱昵称、发件人邮箱账号
            msg['To']=formataddr(["user",receiver])              # 括号里的对应收件人邮箱昵称、收件人邮箱账号
            msg['Subject']="注册邮箱验证"                # 邮件的主题，也可以说是标题

            server=smtplib.SMTP_SSL("smtp.qq.com", 465)  # 发件人邮箱中的SMTP服务器，端口是25
            server.login(self.sender, self.pwd)  # 括号中对应的是发件人邮箱账号、邮箱密码
            server.sendmail(self.sender,[receiver,],msg.as_string())  # 括号中对应的是发件人邮箱账号、收件人邮箱账号、发送邮件
            server.quit()  # 关闭连接
        except Exception:  # 如果 try 中的语句没有执行，则会执行下面的 ret=False
            ret = False
        return ret



def main():
    m = mail("1805549914@qq.com")
    code = m.get_code()
    if m.send(code):
        print "success"
        print code
    else:
        print "error"

if __name__ == '__main__':
    main()
