#!/usr/bin/python
# -*- coding: UTF-8 -*-

import smtplib, random
from email.mime.text import MIMEText
from email.utils import formataddr

arr = [0,1,2,3,4,5,6,7,8,9,'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'];

class mail:
    def __init__(self):
        self.pwd='ayrfzcadudlhbfjd'
        self.sender='1660994874@qq.com'

    def get_code(self):
        res = ""
        for i in range(6):
            idx = random.randint(0, len(arr) - 1)
            res += str(arr[idx]);
        return res;

    def send(self, code, receiver):
        ret=True
        try:
            mail_msg = """
            <!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <title>Document</title>
</head>

<body>
    <div id="qm_con_body">
        <div id="mailContentContainer" class="qmbox qm_con_body_content qqmail_webmail_only" style="">
            <table width="650" bgcolor="#ffffff" align="center" cellpadding="0" cellspacing="0" border="0">
                <tbody>
                    <tr>
                        <td width="30"></td>
                        <td width="590">
                            <table width="100%" cellpadding="0" cellspacing="0" border="0">
                                <tbody>
                                    <tr>
                                        <td>
                                            <div style="border-bottom: 1px solid #f3f3f3;padding-bottom: 7px;">
                                                <table width="100%" cellpadding="0" cellspacing="0" border="0">
                                                    <tbody>
                                                        <tr>
                                                            <td align="left" valign="middle" class="img-wrapper"> <a
                                                                    href="#"
                                                                    rel="noopener" target="_blank"> <img src="https://lanzesucai.oss-cn-beijing.aliyuncs.com/logo.png"
                                                                        border="0"> </a> </td>
                                                            <td width="218" height="69" bgcolor="#FFFFFF" align="right">
                                                                <a href="" target="_blank">
                                                                </a> </td>
                                                        </tr>
                                                    </tbody>
                                                </table>
                                            </div>
                                        </td>
                                    </tr>
                                    <tr>
                                        <td height="8"></td>
                                    </tr>
                                    <tr>
                                        <td>
                                            <table align="right" cellpadding="0" cellspacing="0" border="0">
                                                <tbody>
                                                    <tr>
                                                        <td> </td>
                                                        <td width="15"></td>
                                                        <td align="right"> <a
                                                                href="#"
                                                                style="font-size: 13px; text-decoration: none;"
                                                                rel="noopener" target="_blank">
                                                                <font color="#333">我的蓝沢</font>
                                                            </a> </td>
                                                        <td width="15"></td>
                                                        <td align="right" class="img-wrapper"></td>
                                                        <td width="5"></td>
                                                        <td align="right"> <a
                                                                href="#"
                                                                style="font-size: 13px; text-decoration: none;"
                                                                rel="noopener" target="_blank">
                                                                <font color="#333">蓝沢会员</font>
                                                            </a> </td>
                                                        <td width="15"></td>
                                                        <td align="right" class="img-wrapper"> <img
                                                                src="https://img30.360buyimg.com/mkts/jfs/t2296/85/2552784635/659/95c6496/570e294dNd610fc70.png"
                                                                width="19" height="15" border="0"> </td>
                                                        <td width="5"></td>
                                                        <td align="right"> <a
                                                                href="https://tr.jd.com/jump/transfer?jump_kid=155&amp;jump_klid=82208&amp;jump_gatewayurl=5664316726516209514_default_5_1566431672942&amp;jump_to=https%3A%2F%2Fi-mkt.jd.com%2Fsubscribe%2Findex%3Ftoken%3D2mJchDwfUwn%252B8tcPbkX40cyc1FYTjlCusiHUXa%252FvRx0CWkNr8WBRbaUrlFG5LGA7mji52AFTkM72ir6cXsOmARAj4zICD%252Fcq"
                                                                style="font-size: 13px; text-decoration: none;"
                                                                rel="noopener" target="_blank">
                                                                <font color="#333">退订投诉</font>
                                                            </a> </td>
                                                    </tr>
                                                </tbody>
                                            </table>
                                        </td>
                                    </tr>
                                    <tr>
                                        <td height="20"></td>
                                    </tr>
                                </tbody>
                            </table>
                        </td>
                        <td width="30"></td>
                    </tr>
                </tbody>
            </table>
            <table class="font" width="650" align="center" bgcolor="#ffffff" cellpadding="0" cellspacing="0" border="0"
                style="font-family: Arial, sans-serif;border-collapse: collapse;">
                <tbody>
                    <tr>
                        <td>
                            <table width="591" align="center" cellpadding="0" cellspacing="0" border="0">
                                <tbody>
                                    <tr>
                                        <td height="10"></td>
                                    </tr>
                                    <tr>
                                        <td>
                                            <table cellpadding="0" cellspacing="0" border="0">
                                                <tbody>
                                                    <tr>
                                                        <td
                                                            style="border-collapse: collapse;font-size:13px;line-height: 18px;">
                                                            <b>尊敬的顾客您好：</b><br> 感谢您在冥冥之中选择蓝沢(<a
                                                                href="https://www.lanze.online"
                                                                target="_blank" rel="noopener">
                                                                <font color="#0261b9">lanze.online</font>
                                                            </a>)
                                                            <br> 欢迎注册！</td>
                                                    </tr>
                                                </tbody>
                                            </table>
                                        </td>
                                    </tr>
                                    <tr>
                                        <td height="15"></td>
                                    </tr>
                                    <tr>
                                        <td>
                                            <table width="100%" cellpadding="0" cellspacing="0" border="0">
                                                <tbody>
                                                    <tr>
                                                        <td>
                                                            <table
                                                                style="text-align: center; color:#ffffff;font-size: 20px;"
                                                                bgcolor="#f08595" width="100%" height="31"
                                                                cellpadding="0" cellspacing="0" border="0">
                                                                <tbody>
                                                                    <tr>
                                                                        <td height="31" width="20"></td>
                                                                        <td align="center"> 验证码: <font
                                                                                style="text-center: underline;">
                                                                                </font>
                                                                        </td>
                                         
                                                                        
                                                                    </tr>
                                                                </tbody>
                                                            </table>
                                                            <table
                                                                style="border-right: 1px dashed #dcdcdc;border-left: 1px dashed #dcdcdc;border-bottom: 1px dashed #dcdcdc;color:#f00"
                                                                width="100%" cellpadding="0" cellspacing="0" border="0">
                                                                <tbody>
                                                                    <tr>
                                                                        <td width="25%"></td>
                                                                        <td width="25%"></td>
                                                                        <td width="25%"></td>
                                                                        <td width="25%"></td>
                                                                    </tr>
                                                                    <tr>
                                                                        <td valign="middle" align="center" height="110"
                                                                            style="text-align: center">
																																						<p style="font-size:3em;margin-left:217px">""" + code + """</p>
                                                                             </td>
																																						 
                                                                    </tr>
                                                                </tbody>
                                                            </table>
                                                            <table width="100%" cellpadding="0" cellspacing="0"
                                                                border="0">
                                                                <tbody>
                                                                    <tr>
                                                                        <td height="5"></td>
                                                                    </tr>
                                                                    <tr>
                                                                        <td align="right" style="font-size: 13px;">

                                                                        </td>
                                                                    </tr>
                                                                    <tr>
                                                                        <td height="5"></td>
                                                                    </tr>
                                                                </tbody>
                                                            </table>
                                                            <table width="100%" cellpadding="0" cellspacing="0"
                                                                border="0">
                                                                <tbody>
                                                                    <tr>
                                                                        <td style="font-size: 13px;line-height: 18px;">
                                                                           
                                                                        </td>
                                                                    </tr>
                                                                    <tr>
                                                                        <td height="18"></td>
                                                                    </tr>
                                                                </tbody>
                                                            </table>
                                                        </td>
                                                    </tr>
                                                </tbody>
                                            </table>
                                        </td>
                                    </tr>
                                </tbody>
                            </table>
                        </td>
                    </tr>
                </tbody>
            </table>
            <table width="650" align="center" cellpadding="0" cellspacing="0" border="0">
                <tbody>
                    <tr>
                        <td width="30"></td>
                        <td width="590">
                            <table width="100%" cellpadding="0" cellspacing="0" border="0">
                                <tbody>
                                    <tr>
                                        <td height="20" width="100%"></td>
                                    </tr>
                                </tbody>
                            </table>
                        </td>
                        <td width="30"></td>
                    </tr>
                </tbody>
            </table>
            <table width="650" align="center" cellpadding="0" cellspacing="0" border="0">
                <tbody>
                    <tr>
                        <td width="30"></td>
                        <td width="590">
                            <table width="590" cellpadding="0" cellspacing="0" border="0">
                                <tbody>
                                    <tr>
                                        <td height="1" width="100%" style="border-bottom: 1px solid #f3f3f3;"></td>
                                    </tr>
                                    <tr>
                                        <td height="15" width="100%"></td>
                                    </tr>
                                </tbody>
                            </table>
                            <table align="center" width="590" border="0" cellspacing="0" cellpadding="0">
                                <tbody>
                                    <tr>
                                        <td align="left">
                                            <font style="font-size:12px; line-height:22px" color="#5b5b5b">
                                                您之所以收到这封邮件，是因为您准备注册成为蓝沢的用户。本邮件由蓝沢系统自动发出，请勿直接回复！<br>如果您不愿意继续接收到此类邮件，请点击<a
                                                    href="#"
                                                    rel="noopener" target="_blank">退订本类邮件</a>，在注册中遇到任何问题，请点击<a
                                                    href="#"
                                                    rel="noopener" target="_blank">帮助中心</a>。<br>如果您有任何疑问或建议，请点击<a
                                                    href="#"
                                                    target="_blank" rel="noopener">联系我们</a>。<br>
                                                蓝沢lanze.online是专业的个人交易平台。百万种商品，家用电器、手机数码、服装、电脑、母婴、化妆、图书等十几大类。蓝沢承诺：会充分保护用户的合法权益；支付方式支持货到付款，送货上门及POS刷卡服务！
                                            </font>
                                        </td>
                                    </tr>
																		<br>
                                    <tr>
                                        <td align="left" style="padding-top:6px;padding-button:2px;">
                                            <font style="font-size:12px; line-height:22px" color="#5b5b5b"> 
																						
																						<img
                                                    src="https://img30.360buyimg.com/mkts/jfs/t5971/229/3471985499/2196/40c6093d/5952053eNc8d4d645.png"
                                                    width="19"
                                                    height="12">重要提醒：请认准蓝沢官方发送的短信，谨防虚假诈骗！
                                            </font>
                                        </td>
                                    </tr>
                                    <tr>
                                        <td height="20"></td>
                                    </tr>
                                    <tr>
                                        <td align="center"> <a
                                                href="https://www.lanze.online"
                                                target="_blank" rel="noopener"><img
                                                    src="https://lanzesucai.oss-cn-beijing.aliyuncs.com/logo.png"
                                                    width="138" height="50" border="0" style=" margin-top:5px;"></a>
                                            <br> <a
                                                href="#"
                                                style="font-size: 12px;text-decoration: none" target="_blank"
                                                rel="noopener">
                                                <font color="#323335"> Copyright ©2018-2019 蓝沢lanze.online 版权所有 </font>
                                            </a> </td>
                                    </tr>
                                </tbody>
                            </table>
                        </td>
                        <td width="30"></td>
                    </tr>
                </tbody>
            </table>


            <style type="text/css">
                .qmbox style,
                .qmbox script,
                .qmbox head,
                .qmbox link,
                .qmbox meta {
                    display: none !important;
                }
            </style>
        </div>
    </div>
</body>

</html>
            """
            msg=MIMEText(mail_msg, 'html','utf-8')
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
    m = mail()
    code = m.get_code()
    if m.send(code,'3457554915@qq.com'):
        print "success"
        print code
    else:
        print "error"

if __name__ == '__main__':
    main()
