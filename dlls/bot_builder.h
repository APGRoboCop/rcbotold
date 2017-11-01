class CBotBuild
{
public:
	CBotBuild ( int user3, char *classname, Vector origin )
	{
		iUser3=user3;
		szClassname=gBotGlobals.m_Strings.GetString(szClassname);
		vOrigin=origin;
	}

	void build ()
	{
		BotFunc_NS_MarineBuild(iUser3,szClassname,vOrigin);
	}
private:
	int iUser3;
	char *szClassname;
	Vector vOrigin; // positions for this type of structure
};

class CBotBuilderFile
{
public:
	CBotBuilderFile ( char *file )
	{
		FILE *fp = fopen(file,"r");

		int user3, char *classname, Vector origin;

		iCurrent = 0;
		m_fNextExecute = 0;

		if( fp )
		{			
			while ( !feof(fp) )
			{
				fscanf("%d,%[^,],%0.6f,%0.6f,%0.6f\n",user3,classname,origin.x,origin.y,origin.z);
				
				m_toBuild.push_back(CBotBuild(user3,classname,origin);
			}

			fclose(fp);
		}
	}

	void execute ()
	{
		if ( (iCurrent < m_toBuild.size()) && (m_fNextExecute < gpGlobals->time) )
		{

			CBotBuild *pCurrent = &(m_toBuild[iCurrent++]);

			m_fNextExecute = gpGlobals->time + 1.0;

			pCurrent->build();
		}
	}
private:
	vector<CBotBuild> m_toBuild;
	float m_fNextExecute;
	int iCurrent;
};