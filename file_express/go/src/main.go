package main

import (
	"business"
	"flag"
	"fmt"
	"os"
	"path/filepath"
	"time"

	log4plus "log4go"
)

//版本号
var (
	ver     string = "1.0.17"
	exeName string = "Load so Library"
	pidFile string = ""
)

type Flags struct {
	Help    bool
	Version bool
}

var web *business.WebManager

func (f *Flags) Init() {
	flag.BoolVar(&f.Help, "h", false, "help")
	flag.BoolVar(&f.Version, "V", false, "show version")
}

func (f *Flags) Check() (needReturn bool) {
	flag.Parse()

	if f.Help {
		flag.Usage()
		needReturn = true
	} else if f.Version {
		verString := exeName + " Version: " + ver + "\r\n"
		fmt.Println(verString)
		needReturn = true
	}

	return needReturn
}

var flags *Flags = &Flags{}

func init() {
	flags.Init()
	exeName = getExeName()
	pidFile = GetCurrentDirectory() + "/" + exeName + ".pid"
}

func getExeName() string {
	ret := ""
	ex, err := os.Executable()
	if err == nil {
		ret = filepath.Base(ex)
	}
	return ret
}

func setLog() {
	logJson := "log.json"
	set := false
	if bExist := business.PathExist(logJson); bExist {
		if err := log4plus.SetupLogWithConf(logJson); err == nil {
			set = true
		}
	}

	if !set {
		fileWriter := log4plus.NewFileWriter()
		exeName := getExeName()
		fileWriter.SetPathPattern("./log/" + exeName + "-%Y%M%D.log")
		log4plus.Register(fileWriter)
		log4plus.SetLevel(log4plus.DEBUG)
	}
}

func writePid() {
	SaveFile(fmt.Sprintf("%d", os.Getpid()), pidFile)
}

func main() {

	needReturn := flags.Check()
	if needReturn {
		return
	}

	setLog()
	defer log4plus.Close()
	log4plus.Info("%s Version=%s", getExeName(), ver)

	writePid()
	defer os.Remove(pidFile)
	defer log4plus.Close()

	//fileBusiness := business.New("libharq.so", "127.0.0.1", 41002, "log")

	// version := harq.Version()
	// log4plus.Info("Harq Version=%s", version)

	for {
		time.Sleep(time.Duration(1) * time.Second)
	}

}
