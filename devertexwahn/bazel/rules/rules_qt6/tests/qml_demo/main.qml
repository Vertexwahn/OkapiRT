import QtQuick 2.14
import QtQuick.Window 2.14
import CppObject 1.0
import Sample


Window {
    id: root
    visible: true
    width: 500
    height: 300
    title: qsTr("qml call cpp objects")
    color: "black"

    signal qmlSignalA
    signal qmlSignalB(string str,int value)

    //鼠标点击区域
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        //测试时点击左键或右键
        onClicked: {
            if (mouse.button===Qt.LeftButton) {
                console.log('----qml 点击左键：Cpp发射信号')
                cpp_obj.name="admin"  //修改属性会触发set函数，获取值会触发get函数
                cpp_obj.year=0
                cpp_obj.sendSignal() //调用Q_INVOKABLE宏标记的函数
            } else {
                console.log('----qml 点击右键：QML发射信号')
                root.qmlSignalA()
                root.qmlSignalB('admin',0)
            }
        }
    }

    //作为一个QML对象
    CppObject{
        id:cpp_obj
        //也可以像原生QML对象一样操作，增加属性之类的
        property int counts: 0

        onYearChanged: {
            counts++
            console.log('qml onYearChanged',counts)
        }
        onCountsChanged: {
            console.log('qml onCountsChanged',counts)
        }
    }

    //组件加载完成执行
    Component.onCompleted: {
        //关联信号与信号处理函数的方式同QML中的类型
        //Cpp对象的信号关联到Qml
        //cpp_obj.onCppSignalA.connect(function(){console.log('qml signalA process')})
        cpp_obj.onCppSignalA.connect(()=>console.log('qml signalA process')) //js的lambda
        cpp_obj.onCppSignalB.connect(processB)
        //Qml对象的信号关联到Cpp
        root.onQmlSignalA.connect(cpp_obj.cppSlotA)
        root.onQmlSignalB.connect(cpp_obj.cppSlotB)
    }

    //定义的函数可以作为槽函数
    function processB(str,value){
        console.log('qml function processB',str,value)
    }
}
